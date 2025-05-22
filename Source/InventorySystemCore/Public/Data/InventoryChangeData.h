// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "InventoryChangeData.generated.h"

struct FInventoryEntry;
class UItemInstance;

/**
 * Defines the types of changes that can occur to inventory entries
 */
UENUM(BlueprintType)
enum class EInventoryChangeType : uint8
{
	Added, ///< Item was added to the inventory
	Removed, ///< Item was removed from the inventory
	Modified ///< Item's properties were modified (e.g. stack count)
};

/**
 * @struct FInventoryChangeData
 * @see UItemInstance, FInventoryEntry, EInventoryChangeType
 * @brief Represents a data payload for tracking inventory item changes including addition, removal, and modification of items
 * @details Contains information about the affected item instance, its index in the inventory, the type of change, 
 *		  and stack count changes before and after the modification occurred. Used for notifying systems about
 *		  inventory state changes and maintaining synchronization.
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryChangeData
{
	GENERATED_BODY()

	FInventoryChangeData() {}

	/**
	 * Constructs an inventory change data instance with the specified parameters
	 * @param InIndex The index of the inventory entry that was modified
	 * @param Entry The inventory entry containing the item instance and stack count
	 * @param InChangeType The type of modification that occurred
	 */
	FInventoryChangeData(const int32 InIndex, const FInventoryEntry& Entry, const EInventoryChangeType InChangeType = EInventoryChangeType::Added);

	/** 
	 * Zero-based index of the modified inventory entry
	 * Set to INDEX_NONE (-1) when the entry is invalid
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Index = INDEX_NONE;

	/** 
	 * Reference to the affected item instance
	 * Contains the item's definition and properties
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UItemInstance> Instance = nullptr;

	/** 
	 * Specifies whether the item was added, removed, or modified
	 * Defaults to Added when not specified
	 */
	UPROPERTY(BlueprintReadOnly)
	EInventoryChangeType ChangeType = EInventoryChangeType::Added;

	/** 
	 * Stack count before the inventory modification occurred
	 * Used to track quantity changes
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 OldCount = 0;

	/** 
	 * Stack count after the inventory modification occurred
	 * Represents the current quantity
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 NewCount = 0;

	/**
	 * Checks if this inventory change data is valid
	 * @return True if the index is valid (not INDEX_NONE), false otherwise
	 */
	bool IsValid() const { return Index != INDEX_NONE; }
};