#pragma once

#include "Definitions/ItemDefinition.h"

#include "InventorySet_ItemSet.generated.h"


/**
 * FInventorySet_ItemSet
 *
 *	Data used by the InventorySet to give items
 */
USTRUCT(BlueprintType)
struct FInventorySet_ItemSet
{
	GENERATED_BODY()

	bool IsEquipable() const;
	
	
	// Item's definition to give
	UPROPERTY(EditDefaultsOnly, DisplayName = "Definition")
	TSubclassOf<UItemDefinition> ItemDefinition = nullptr;

	// Quantity to give
	UPROPERTY(EditDefaultsOnly)
	int Quantity = 1.0f;
};
