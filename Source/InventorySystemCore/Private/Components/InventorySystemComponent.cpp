// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventorySystemComponent.h"

#include "Data/InventoryCache.h"
#include "Data/InventoryEntry.h"
#include "Engine/ActorChannel.h"
#include "Instances/ItemInstance.h"
#include "Net/UnrealNetwork.h"


UInventorySystemComponent::UInventorySystemComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.Get()), InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
	bReplicateUsingRegisteredSubObjectList = true;
	bWantsInitializeComponent = true;
}

UInventorySystemComponent::~UInventorySystemComponent()
{
	if (Cache.IsValid())
		Cache->Clear();
}

void UInventorySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache initialization
	Cache = NewObject<UInventoryCache>(this);
}

void UInventorySystemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UInventorySystemComponent::ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
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

UItemInstance* UInventorySystemComponent::AddItemDefinition(TSubclassOf<UItemDefinition> ItemDefinition, int32 Count)
{
	if (!IsValid(ItemDefinition))
	{
		return nullptr;
	}

	UItemInstance* Instance = InventoryList.Add(ItemDefinition, Count);

	// Register new instances for replication
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(Instance))
	{
		AddReplicatedSubObject(Instance);
	}

	return Instance;
}

void UInventorySystemComponent::AddItemInstance(UItemInstance* ItemInstance)
{
	// TODO : Add possibility to add an existing instance of an item in the inventory to handle modifiers and upgrades
	unimplemented();
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

int32 UInventorySystemComponent::GetTotalItemCountByDefinition(TSubclassOf<UItemDefinition> ItemDefinitionClass) const
{
	int32 TotalCount = 0;
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (const UItemInstance* Instance = Entry.Instance; IsValid(Instance))
		{
			if (Instance->GetDefinitionClass() == ItemDefinitionClass)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

UItemDefinition* UInventorySystemComponent::GetItemDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const
{
	if(Cache.IsValid())
	{
		return Cache->GetCachedDefinition(ItemDefinitionClass);
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
