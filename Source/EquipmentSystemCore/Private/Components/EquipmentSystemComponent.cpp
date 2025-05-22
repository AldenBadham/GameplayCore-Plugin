// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/EquipmentSystemComponent.h"

#include "Data/EquipmentCache.h"
#include "Data/EquipmentEntry.h"
#include "Engine/ActorChannel.h"
#include "Instances/EquipmentInstance.h"
#include "Net/UnrealNetwork.h"

UEquipmentSystemComponent::UEquipmentSystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UEquipmentSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
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
	Cache = NewObject<UEquipmentCache>(this, "Cache");
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

UEquipmentInstance* UEquipmentSystemComponent::EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, UItemInstance* SourceItemInstance)
{
	if (IsValid(EquipmentDefinition))
	{
		if (UEquipmentInstance* Instance = EquipmentList.Add(EquipmentDefinition, SourceItemInstance))
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

UEquipmentInstance* UEquipmentSystemComponent::GetInstanceOfType(const TSubclassOf<UEquipmentInstance>& InstanceClass)
{
	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
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

TArray<UEquipmentInstance*> UEquipmentSystemComponent::GetAllInstancesOfType(const TSubclassOf<UEquipmentInstance>& InstanceClass)
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

UEquipmentDefinition* UEquipmentSystemComponent::GetCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class) const
{
	if (IsValid(Cache))
	{
		return Cache->GetCachedDefinition(Class);
	}
	if (IsValid(Class))
	{
		return NewObject<UEquipmentDefinition>(GetOuter(), Class);
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


void UEquipmentSystemComponent::PostEquipmentEquipped(const FEquipmentChangeData& Data)
{
	OnEquipmentEquipped.Broadcast(Data);
}

void UEquipmentSystemComponent::PostEquipmentUnequipped(const FEquipmentChangeData& Data)
{
	OnEquipmentUnequipped.Broadcast(Data);
}

void UEquipmentSystemComponent::PostEquipmentChanged(const FEquipmentChangeData& Data)
{
	OnEquipmentChanged.Broadcast(Data);
}