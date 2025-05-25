// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/EquipmentList.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/EquipmentSystemComponent.h"
#include "Data/AbilitySet.h"
#include "Data/AbilitySetHandles.h"
#include "Data/EquipmentChangeData.h"
#include "GameplayTags/EquipmentGameplayTags.h"
#include "Instances/EquipmentInstance.h"
#include "Log/EquipmentSystemLog.h"

FEquipmentList::FEquipmentList()
{
}

FEquipmentList::FEquipmentList(UEquipmentSystemComponent* InOwnerComponent)
	: OwnerComponent(InOwnerComponent)
{
}

FEquipmentList::~FEquipmentList()
{
	Entries.Empty();
}

void FEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		if (FEquipmentEntry& Entry = Entries[Index]; Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
			Entry.LastInstance = nullptr;

			Internal_OnEntryRemoved(Index, Entry);
		}
	}
}

void FEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		if (FEquipmentEntry& Entry = Entries[Index]; Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
			Entry.LastInstance = MakeWeakObjectPtr<UEquipmentInstance>(Entry.Instance);

			Internal_OnEntryAdded(Index, Entry);
		}
	}
}

void FEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		if (Entries.IsValidIndex(Index))
		{
			FEquipmentEntry& Entry = Entries[Index];

			if (IsValid(Entry.Instance))
			{
				Entry.Instance->OnEquipped();
			}

			if (Entry.Instance != Entry.LastInstance.Get() && Entry.LastInstance.IsValid())
			{
				Entry.LastInstance->OnUnequipped();
				Entry.LastInstance = MakeWeakObjectPtr<UEquipmentInstance>(Entry.Instance);
			}

			Internal_OnEntryChanged(Index, Entry);
		}
	}
}

FEquipmentResult FEquipmentList::Add(const TSubclassOf<UEquipmentDefinition>& DefinitionClass, UItemInstance* SourceItemInstance)
{
	FEquipmentResult Result;
	if (DefinitionClass == nullptr || !IsValid(OwnerComponent))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to instantiate an equipment instance from invalid definition class."));
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_MissingDefinition;
		return Result;
	}

	AActor* OwnerActor = OwnerComponent->GetOwner();
	if (!OwnerActor->HasAuthority())
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to instantiate an equipment instance from invalid definition class."));
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_NotAuthority;
		return Result;
	}

	UEquipmentDefinition* CachedDefinition = OwnerComponent->GetCachedDefinition(DefinitionClass);
	if (!CachedDefinition->CanBeEquipped(OwnerComponent, Result.FailureReason))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to instantiate an equipment instance from %s, but the definition conditions are not met."), *GetNameSafe(DefinitionClass));
		if (!Result.FailureReason.IsValid())
			Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_DefinitionRefused;
		return Result;
	}

	// Prepare an instance type to spawn, use the default one by default
	TSubclassOf<UEquipmentInstance> InstanceType = CachedDefinition->InstanceClass;
	if (InstanceType == nullptr)
	{
		InstanceType = UEquipmentDefinition::StaticClass();
	}

	// Add default entry
	FEquipmentEntry& Entry = Entries.AddDefaulted_GetRef();

	Entry.EquipmentDefinition = DefinitionClass;
	Entry.Instance = NewObject<UEquipmentInstance>(OwnerActor, InstanceType);

	UEquipmentInstance* Instance = Entry.Instance;
	Instance->SetDefinition(CachedDefinition);
	Instance->SetInstigator(OwnerActor);
	Instance->SetSourceItem(SourceItemInstance);

	Result.Instance = Instance;

	// Give the ability sets
	if (UAbilitySystemComponent* AbilitySystemComp = GetAbilitySystemComponent())
	{
		for (const TObjectPtr<const UAbilitySet>& AbilitySet : CachedDefinition->AbilitySets)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComp, &Entry.Handles, Instance);
		}
	}

	// Ask the instance to spawn attachment actors
	Instance->SpawnActors(CachedDefinition->ActorsToSpawn);
	Instance->OnSpawned();

	// Mark the item dirty for the serializer replication
	MarkItemDirty(Entry);

	return Result;
}

void FEquipmentList::Remove(UEquipmentInstance* Instance, FGameplayTag& OutFailureReason)
{
	for (auto EntryIterator = Entries.CreateIterator(); EntryIterator; ++EntryIterator)
	{
		if (FEquipmentEntry& Entry = *EntryIterator; Entry.Instance == Instance)
		{
			if (UAbilitySystemComponent* AbilitySystemComp = GetAbilitySystemComponent())
			{
				Entry.Handles.TakeFromAbilitySystem(AbilitySystemComp);
			}

			// Ask the instance to destroy its attachment actors
			Instance->DestroyActors();

			EntryIterator.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FEquipmentList::Internal_OnEntryChanged(const int32 Index, const FEquipmentEntry& Entry) const
{
	FEquipmentChangeData Data;
	Data.Index = Index;
	Data.NewInstance = Entry.Instance;
	Data.OldInstance = nullptr;
	Data.ChangeType = EEquipmentChangeType::Modified;

	OwnerComponent->PostEquipmentChanged(Data);
}

void FEquipmentList::Internal_OnEntryAdded(const int32 Index, const FEquipmentEntry& Entry) const
{
	FEquipmentChangeData Data;
	Data.Index = Index;
	Data.NewInstance = Entry.Instance;
	Data.OldInstance = nullptr;
	Data.ChangeType = EEquipmentChangeType::Equipped;

	OwnerComponent->PostEquipmentEquipped(Data);
	OwnerComponent->PostEquipmentChanged(Data);
}

void FEquipmentList::Internal_OnEntryRemoved(const int32 Index, const FEquipmentEntry& Entry) const
{
	FEquipmentChangeData Data;
	Data.Index = Index;
	Data.NewInstance = nullptr;
	Data.OldInstance = Entry.Instance;
	Data.ChangeType = EEquipmentChangeType::Unequipped;

	OwnerComponent->PostEquipmentUnequipped(Data);
	OwnerComponent->PostEquipmentChanged(Data);
}

UAbilitySystemComponent* FEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor);
}
