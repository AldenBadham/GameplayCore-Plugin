﻿#include "Components/EquipmentSlotComponent.h"

#include "Components/EquipmentSystemComponent.h"
#include "Data/EquipmentSlotMapData.h"
#include "Definitions/Fragments/ItemFragment_Equippable.h"
#include "Instances/EquipmentInstance.h"
#include "Instances/ItemInstance.h"
#include "Net/UnrealNetwork.h"

UEquipmentSlotComponent::UEquipmentSlotComponent(const FObjectInitializer& ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	bWantsInitializeComponent = true;
}

void UEquipmentSlotComponent::BeginPlay()
{
	Slots.Empty();

	for (const FGameplayTag& SlotTag : SlotMap->Slots)
	{
		Slots.Add({SlotTag});
	}

	Super::BeginPlay();
}

void UEquipmentSlotComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
}

void UEquipmentSlotComponent::Server_EquipItemAtSlot_Implementation(const FGameplayTag SlotTag)
{
	if (const FEquipmentSlotSet* InstanceSet = FindInstanceSetForSlot(SlotTag))
	{
		if (!IsValid(InstanceSet->ItemInstance))
		{
			return;
		}

		UnequipItemInSlot(SlotTag);
		EquipItemInSlot(SlotTag);
	}
}

void UEquipmentSlotComponent::AddItemToSlot(const FGameplayTag SlotTag, UItemInstance* Item)
{
	if (IsValidSlot(SlotTag) && Item != nullptr)
	{
		if (FEquipmentSlotSet* InstanceSet = FindInstanceSetForSlot(SlotTag))
		{
			InstanceSet->ItemInstance = Item;
			OnRep_Slots();

			// OnItemAddedToSlot.Broadcast(SlotIndex, Item);
		}
	}
}

UItemInstance* UEquipmentSlotComponent::RemoveItemFromSlot(const FGameplayTag SlotTag)
{
	UItemInstance* Result = nullptr;

	UnequipItemInSlot(SlotTag);

	if (IsValidSlot(SlotTag))
	{
		if (FEquipmentSlotSet* InstanceSet = FindInstanceSetForSlot(SlotTag))
		{
			Result = InstanceSet->ItemInstance;
			InstanceSet->ItemInstance = nullptr;

			OnRep_Slots();
		}
	}

	return Result;
}

void UEquipmentSlotComponent::EquipItemInSlot(const FGameplayTag SlotTag)
{
	check(IsValidSlot(SlotTag));

	// Check if we've already an item in the slot
	if (FindInstanceSetForSlot(SlotTag) != nullptr)
	{
		UnequipItemInSlot(SlotTag);
	}

	UEquipmentSystemComponent* EquipmentSystemComp = FindEquipmentSystem();
	if (!EquipmentSystemComp)
	{
		return;
	}

	if (const FEquipmentSlotSet* InstanceSet = FindInstanceSetForSlot(SlotTag))
	{
		UItemInstance* Item = InstanceSet->ItemInstance;

		if (const UItemFragment_EquipableItem* EquipFragment = Item->FindFragmentByClass<UItemFragment_EquipableItem>())
		{
			if (const TSubclassOf<UEquipmentDefinition> EquipmentDefinition = EquipFragment->EquipmentDefinition; IsValid(EquipmentDefinition))
			{
				if (UEquipmentInstance* NewInstance = EquipmentSystemComp->EquipItem(EquipmentDefinition); NewInstance != nullptr)
				{
					NewInstance->SetSourceItem(Item);
				}
			}
		}
	}
}

void UEquipmentSlotComponent::UnequipItemInSlot(const FGameplayTag SlotTag)
{
	if (!IsValidSlot(SlotTag))
	{
		return;
	}

	if (UEquipmentSystemComponent* EquipmentSystem = FindEquipmentSystem())
	{
		if (const FEquipmentSlotSet* InstanceSet = FindInstanceSetForSlot(SlotTag))
		{
			if (UEquipmentInstance* EquipmentInstance = EquipmentSystem->GetInstanceFromItem(InstanceSet->ItemInstance))
			{
				EquipmentSystem->UnequipItem(EquipmentInstance);

				// OnSlotUnequipped.Broadcast(ActiveSlotIndex);
			}
		}
	}
}

FEquipmentSlotSet* UEquipmentSlotComponent::FindInstanceSetForSlot(FGameplayTag SlotTag)
{
	return Slots.FindByPredicate([SlotTag](const FEquipmentSlotSet& Set) { return Set.SlotTag == SlotTag; });
}

bool UEquipmentSlotComponent::IsValidSlot(const FGameplayTag SlotTag) const
{
	return SlotMap->Slots.Contains(SlotTag);
}

UEquipmentSystemComponent* UEquipmentSlotComponent::FindEquipmentSystem() const
{
	if (const AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (const APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<UEquipmentSystemComponent>();
		}
	}
	return nullptr;
}

void UEquipmentSlotComponent::OnRep_Slots() {}