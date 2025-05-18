// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/EquipmentList.h"

#include "Components/EquipmentSystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Data/AbilitySet.h"
#include "Data/AbilitySetHandles.h"
#include "Instances/EquipmentInstance.h"

FEquipmentList::FEquipmentList()
	: OwnerComponent(nullptr)
{
}

FEquipmentList::FEquipmentList(UEquipmentSystemComponent* InOwnerComponent) : Entries(), OwnerComponent(InOwnerComponent)
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
		if (const FEquipmentEntry& Entry = Entries[Index]; Entry.Instance != nullptr)
		{
			Entry.Instance->OnUnequipped();
		}
	}
}

void FEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		const FEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance != nullptr)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

UEquipmentInstance* FEquipmentList::Add(const TSubclassOf<UEquipmentDefinition>& DefinitionClass, int32 Count)
{
	if (DefinitionClass == nullptr || !IsValid(OwnerComponent))
	{
		return nullptr;
	}

	AActor* OwnerActor = OwnerComponent->GetOwner();
	if (!OwnerActor->HasAuthority())
	{
		return nullptr;
	}

	
	UEquipmentDefinition* CachedDefinition = OwnerComponent->GetEquipmentDefinition(DefinitionClass);
	
	if(!CachedDefinition->CanEquip(OwnerComponent))
		return nullptr;

	// Prepare instance type to spawn, use default one by default
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
	//Instance->AsyncLoadAssets();
	Instance->SetDefinition(CachedDefinition);
	Instance->SetInstigator(OwnerActor);

	// Give ability set
	if(UAbilitySystemComponent* AbilitySystemComp = GetAbilitySystemComponent())
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

	return Instance;
}

void FEquipmentList::Remove(UEquipmentInstance* Instance)
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

UAbilitySystemComponent* FEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	
	AActor* OwningActor = OwnerComponent->GetOwner();
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor);
}
