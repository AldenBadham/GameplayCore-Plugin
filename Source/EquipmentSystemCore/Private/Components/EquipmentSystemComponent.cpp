// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/EquipmentSystemComponent.h"

#include "Data/EquipmentCache.h"
#include "Data/EquipmentEntry.h"
#include "Engine/ActorChannel.h"
#include "Instances/EquipmentInstance.h"
#include "Net/UnrealNetwork.h"

UEquipmentSystemComponent::UEquipmentSystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;

	// Cache initialization
	Cache = NewObject<UEquipmentCache>(this, UEquipmentCache::StaticClass(), "Cache");
}

UEquipmentSystemComponent::~UEquipmentSystemComponent()
{
	if (Cache.IsValid())
	{
		Cache->Clear();
	}
}

void UEquipmentSystemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

bool UEquipmentSystemComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bReplicated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance; IsValid(Instance))
		{
			bReplicated |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bReplicated;
}

void UEquipmentSystemComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register all equipment instances ready for replication with the actor component
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FEquipmentEntry& Entry : EquipmentList.Entries)
		{
			if (UEquipmentInstance* Instance = Entry.Instance; IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

void UEquipmentSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache initialization
	Cache = NewObject<UEquipmentCache>(this);
}

void UEquipmentSystemComponent::UninitializeComponent()
{
	TArray<UEquipmentInstance*> Instances;

	// Gathering all instances before removal to avoid side effects affecting the equipment list iterator
	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		Instances.Add(Entry.Instance);
	}

	for (UEquipmentInstance* EquipInstance : Instances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

UEquipmentInstance* UEquipmentSystemComponent::EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition)
{
	if (IsValid(EquipmentDefinition))
	{
		if (UEquipmentInstance* Instance = EquipmentList.Add(EquipmentDefinition))
		{
			Instance->OnEquipped();

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Instance);
			}

			return Instance;
		}
	}
	return nullptr;
}

void UEquipmentSystemComponent::UnequipItem(UEquipmentInstance* ItemInstance)
{
	if (IsValid(ItemInstance))
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		ItemInstance->OnUnequipped();
		EquipmentList.Remove(ItemInstance);
	}
}

UEquipmentInstance* UEquipmentSystemComponent::GetInstanceOfType(TSubclassOf<UEquipmentInstance> InstanceClass)
{
	for (FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceClass))
			{
				return Instance;
			}
		}
	}
	return nullptr;
}

TArray<UEquipmentInstance*> UEquipmentSystemComponent::GetAllEquipmentInstancesOfType(TSubclassOf<UEquipmentInstance> InstanceClass)
{
	TArray<UEquipmentInstance*> Results;
	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceClass))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}

UEquipmentDefinition* UEquipmentSystemComponent::GetEquipmentDefinition(const TSubclassOf<UEquipmentDefinition>& Class) const
{
	if (Cache.IsValid())
	{
		return Cache->GetCachedDefinition(Class);
	}
	return nullptr;
}

UEquipmentInstance* UEquipmentSystemComponent::GetInstanceFromItem(UItemInstance* ItemInstance)
{
	for (FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->GetSourceItem() == ItemInstance)
			{
				return Instance;
			}
		}
	}
	return nullptr;
}
