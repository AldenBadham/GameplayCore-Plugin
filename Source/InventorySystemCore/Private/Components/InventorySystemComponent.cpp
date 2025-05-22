// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/InventorySystemComponent.h"

#include "Data/InventoryCache.h"
#include "Data/InventoryEntry.h"
#include "Engine/ActorChannel.h"
#include "Instances/ItemInstance.h"
#include "Net/UnrealNetwork.h"

UInventorySystemComponent::UInventorySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get()), InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	bReplicateUsingRegisteredSubObjectList = true;
	bWantsInitializeComponent = true;
}

void UInventorySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache initialization
	Cache = NewObject<UInventoryCache>(this);
}

void UInventorySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UInventorySystemComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bHasReplicated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (UItemInstance* Instance = Entry.Instance; IsValid(Instance))
		{
			bHasReplicated |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bHasReplicated;
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

TArray<UItemInstance*> UInventorySystemComponent::AddItemDefinition(const TSubclassOf<UItemDefinition> ItemDefinition, const int32 Count)
{
	if (!IsValid(ItemDefinition))
	{
		return {};
	}

	TArray<UItemInstance*> Instances = InventoryList.Add(ItemDefinition, Count);

	// Register new instances for replication
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		for (UItemInstance* Instance : Instances)
		{
			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}

	return Instances;
}

void UInventorySystemComponent::AddItemInstance(UItemInstance* ItemInstance)
{
	// Verification and authority
	if (!IsValid(ItemInstance))
	{
		return;
	}

	if (const AActor* OwnerActor = GetOwner(); !OwnerActor->HasAuthority())
	{
		return;
	}

	// InventoryList delegation
	InventoryList.AddItemInstance(ItemInstance);

	// Replication management at component level
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		if (!IsReplicatedSubObjectRegistered(ItemInstance))
		{
			AddReplicatedSubObject(ItemInstance);
		}
	}
}

void UInventorySystemComponent::RemoveItemInstance(UItemInstance* ItemInstance)
{
	InventoryList.Remove(ItemInstance);

	if (ItemInstance && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
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