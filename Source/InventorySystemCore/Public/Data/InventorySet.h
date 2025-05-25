// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "InventoryList.h"
#include "Engine/DataAsset.h"

#include "InventorySet.generated.h"

struct FGameplayTag;
struct FInventorySet_ItemSet;
class UItemDefinition;
class UInventorySystemComponent;

/**
 * @class UInventorySet
 * @see UPrimaryDataAsset
 * @brief Data asset storing a predefined set of items to be granted together
 * @details Used to define groups of items that can be given to an inventory system component at once.
 * Commonly used for starting equipment, loot drops, or quest rewards.
 */
UCLASS()
class INVENTORYSYSTEMCORE_API UInventorySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Grants all items defined in this set to the specified inventory system component
	 * @param InventorySystemComp The target inventory component that will receive the items
	 * @see UInventorySystemComponent
	 */
	FInventoryAddResult GiveToInventorySystem(UInventorySystemComponent* InventorySystemComp);

protected:
	/**
	 * Collection of item definitions and their quantities to be granted together
	 * Each entry specifies an item type and how many of that item should be given
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Items", meta = (TitleProperty = "[{Quantity}] {ItemDefinition}"))
	TArray<FInventorySet_ItemSet> Items;
};
