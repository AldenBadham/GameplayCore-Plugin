// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/InventorySystemComponent.h"

#include "GameplayTagContainer.h"
#include "Data/InventoryCache.h"
#include "Data/InventoryEntry.h"
#include "Engine/ActorChannel.h"
#include "GameplayTags/InventoryGameplayTags.h"
#include "Instances/ItemInstance.h"
#include "Net/UnrealNetwork.h"

UInventorySystemComponent::UInventorySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get()), InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UInventorySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UInventorySystemComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bReplicated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (UItemInstance* Instance = Entry.Instance; IsValid(Instance))
		{
			bReplicated |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bReplicated;
}

void UInventorySystemComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register all item instances ready for replication with the actor component
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FInventoryEntry& Entry : InventoryList.Entries)
		{
			if (UItemInstance* Instance = Entry.Instance; IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

void UInventorySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache initialization
	Cache = NewObject<UInventoryCache>(this);
}

void UInventorySystemComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

FInventoryAddResult UInventorySystemComponent::TryAddItemDefinition(const TSubclassOf<UItemDefinition>& ItemDefinition, const int32 Count)
{
	FInventoryAddResult Result;
	if (!IsValid(ItemDefinition))
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_NullDefinition;
		return Result;
	}
	if (Count <= 0)
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidCount;
		return Result;
	}

	return Internal_AddItemDefinition(ItemDefinition, Count);
}

FInventoryAddResult UInventorySystemComponent::TryAddItemInstance(UItemInstance* ItemInstance, int32 StackCount)
{
	FInventoryAddResult Result;
	if (!IsValid(ItemInstance))
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_NullInstance;
		return Result;
	}
	if (StackCount <= 0)
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidCount;
		return Result;
	}

	return Internal_AddItemInstance(ItemInstance, StackCount);
}

bool UInventorySystemComponent::TryRemoveItemInstance(UItemInstance* ItemInstance, FGameplayTag& OutFailureReason)
{
	if (!IsValid(ItemInstance))
	{
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_NullInstance;
		return false;
	}

	return Internal_RemoveItemInstance(ItemInstance, OutFailureReason);
}


FInventoryAddResult UInventorySystemComponent::Internal_AddItemDefinition(const TSubclassOf<UItemDefinition>& ItemDefinition, int32 Count)
{
	FInventoryAddResult Result = InventoryList.Add(ItemDefinition, Count);

	if (!Result.Succeeded())
	{
		return Result;
	}

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		for (UItemInstance* Instance : Result.Instances)
		{
			if (IsValid(Instance) && !IsReplicatedSubObjectRegistered(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}

	return Result;
}

FInventoryAddResult UInventorySystemComponent::Internal_AddItemInstance(UItemInstance* ItemInstance, const int32 StackCount)
{
	FInventoryAddResult Result = InventoryList.AddItemInstance(ItemInstance, StackCount);

	if (Result.Succeeded() && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		if (!IsReplicatedSubObjectRegistered(ItemInstance))
		{
			AddReplicatedSubObject(ItemInstance);
		}
	}

	return Result;
}

bool UInventorySystemComponent::Internal_RemoveItemInstance(UItemInstance* ItemInstance, FGameplayTag& OutFailureReason)
{
	InventoryList.Remove(ItemInstance, OutFailureReason);

	if (IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}

	OutFailureReason = FGameplayTag::EmptyTag;
	return true;
}

TArray<FInventoryEntryHandle> UInventorySystemComponent::GetAllStacks() const
{
	return InventoryList.GetAllHandles();
}

int32 UInventorySystemComponent::GetStackCountByDefinition(const TSubclassOf<UItemDefinition> DefinitionClass) const
{
	int32 Count = 0;
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (const UItemInstance* Instance = Entry.Instance; IsValid(Instance))
		{
			if (Instance->GetDefinitionClass() == DefinitionClass)
			{
				++Count;
			}
		}
	}

	return Count;
}

int32 UInventorySystemComponent::GetTotalCountByDefinition(const TSubclassOf<UItemDefinition> ItemDefinitionClass) const
{
	int32 TotalCount = 0;
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (const UItemInstance* Instance = Entry.Instance; IsValid(Instance))
		{
			if (Instance->GetDefinitionClass() == ItemDefinitionClass)
			{
				TotalCount += Entry.StackCount;
			}
		}
	}

	return TotalCount;
}

UItemDefinition* UInventorySystemComponent::GetCachedDefinition(const TSubclassOf<UItemDefinition>& Class) const
{
	if (IsValid(Cache))
	{
		return Cache->GetCachedDefinition(Class);
	}
	if (IsValid(Class))
	{
		return NewObject<UItemDefinition>(GetOuter(), Class);
	}
	return nullptr;
}

void UInventorySystemComponent::PostInventoryEntryAdded(const FInventoryChangeData& Data)
{
	OnInventoryEntryAdded.Broadcast(Data);
}

void UInventorySystemComponent::PostInventoryEntryRemoved(const FInventoryChangeData& Data)
{
	OnInventoryEntryRemoved.Broadcast(Data);
}

void UInventorySystemComponent::PostInventoryEntryChanged(const FInventoryChangeData& Data)
{
	OnInventoryEntryChanged.Broadcast(Data);
}

void UInventorySystemComponent::PostInventoryChanged(const FInventoryChangeData& Data)
{
	OnInventoryChanged.Broadcast(Data);
}
