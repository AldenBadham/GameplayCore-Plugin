// Fill out your copyright notice in the Description page of Project Settings.

#include "Definitions/Fragments/ItemFragment_Equippable.h"

#include "Components/EquipmentSystemComponent.h"
#include "Instances/ItemInstance.h"

void UItemFragment_EquipableItem::OnInstanceCreated(UItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	if (UEquipmentSystemComponent* EquipmentSystemComp = FindEquipmentComponent(); IsValid(EquipmentSystemComp))
	{
	}
}

UEquipmentSystemComponent* UItemFragment_EquipableItem::FindEquipmentComponent() const
{
	check(OwningInstance);

	if (const APlayerController* PlayerController = OwningInstance->GetOwningController(); IsValid(PlayerController))
	{
		if (const APawn* OwnedPawn = PlayerController->GetPawn(); IsValid(OwnedPawn))
		{
			return OwnedPawn->GetComponentByClass<UEquipmentSystemComponent>();
		}
	}
	return nullptr;
}
