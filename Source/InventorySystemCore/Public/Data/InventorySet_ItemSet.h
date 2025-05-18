#pragma once

#include "Definitions/ItemDefinition.h"

#include "InventorySet_ItemSet.generated.h"

/**
 * @class FInventorySet_ItemSet
 * @see UItemDefinition
 * @brief Data structure used by the InventorySet to manage item definitions and quantities
 * @details Represents a set of items to be added to an inventory, containing both the item definition
 * and the quantity to be given. Used primarily for initial inventory setup and item grants.
 */
USTRUCT(BlueprintType)
struct FInventorySet_ItemSet
{
	GENERATED_BODY()

	/**
	 * Checks if the item in this set can be equipped
	 * @return True if the item definition is valid and represents an equippable item
	 */
	bool IsEquipable() const;

	/** The item definition class that defines the type of item to be given */
	UPROPERTY(EditDefaultsOnly, DisplayName = "Definition")
	TSubclassOf<UItemDefinition> ItemDefinition = nullptr;

	/** The number of items of this type to give */
	UPROPERTY(EditDefaultsOnly)
	int Quantity = 1.0f;
};
