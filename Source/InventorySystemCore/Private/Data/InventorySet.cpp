// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/InventorySet.h"

#include "Components/InventorySystemComponent.h"
#include "Data/InventorySet_ItemSet.h"
#include "Log/InventorySystemLog.h"

void UInventorySet::GiveToInventorySystem(UInventorySystemComponent* InventorySystemComp)
{
	if (!IsValid(InventorySystemComp))
	{
		UE_LOG(LogInventorySystem, Error, TEXT("Tried to give InventorySet [%s] to an invalid InventorySystemComponent"), *GetFName().ToString());
	}

	for (auto& [ItemDefinition, Quantity] : Items)
	{
		if (!IsValid(ItemDefinition) || Quantity <= 0)
		{
			continue;
		}

		InventorySystemComp->AddItemDefinition(ItemDefinition, Quantity);
	}
}
