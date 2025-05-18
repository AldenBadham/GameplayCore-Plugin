// Fill out your copyright notice in the Description page of Project Settings.


#include "Definitions/Fragments/EquipmentFragment_Blessable.h"

#include "Instances/EquipmentInstance.h"
#include "Instances/Components/EquipmentComponent_Blessable.h"

void UEquipmentFragment_Blessable::OnInstanceCreated(UEquipmentInstance* Instance) const
{
	Super::OnInstanceCreated(Instance);

	if (const auto Component = Instance->AddComponent<UEquipmentComponent_Blessable>(); IsValid(Component))
	{
		Component->bIsBlessed = bBlessedOnGive;
	}
}
