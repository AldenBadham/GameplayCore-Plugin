// Fill out your copyright notice in the Description page of Project Settings.


#include "Definitions/Fragments/ItemFragment_Consumable.h"
#include "Instances/ItemInstance.h"

#include "Instances/Components/ItemComponent_Consumable.h"


void UItemFragment_Consumable::OnInstanceCreated(UItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	if (const auto Component = Instance->AddComponent<UItemComponent_Consumable>(); IsValid(Component))
	{
		Component->CurrentUse = MaxUsesCount;
	}
}
