// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"

#include "InventoryEntryHandle.generated.h"

class UItemInstance;

/**
 * @struct FInventoryEntryHandle
 * @see UItemInstance
 * @brief Represents a handle to an inventory entry, providing access to its index, item instance, and stack count.
 * @details This structure acts as a lightweight wrapper around inventory entry data, allowing access to the entry's
 * position in the inventory, its associated item instance, and the current stack count. Used throughout the inventory
 * system to reference and manipulate inventory entries.
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryEntryHandle
{
	GENERATED_BODY()

	FInventoryEntryHandle()
		: ItemInstance(nullptr), StackCount(0)
	{
	}

	/**
	 * Creates a handle from an existing inventory entry
	 * @param InIndex Index of the entry in the inventory
	 * @param InEntry The inventory entry to reference
	 */
	FInventoryEntryHandle(const int32 InIndex, const FInventoryEntry& InEntry)
		: EntryIndex(InIndex)
	{
		EntryIndex = InIndex;
		ItemInstance = InEntry.Instance;
		StackCount = InEntry.StackCount;
	}

	/**
	 * Creates a handle with specified values
	 * @param InIndex Index of the entry in the inventory
	 * @param InInstance The item instance to reference
	 * @param InStackCount Number of items in the stack
	 */
	FInventoryEntryHandle(const int32 InIndex, UItemInstance* InInstance, const int32 InStackCount)
		: EntryIndex(InIndex), ItemInstance(InInstance), StackCount(InStackCount)
	{
	}

	/**
	 * Checks if this handle references a valid inventory entry
	 * @return true if the handle points to a valid entry, false otherwise
	 */
	bool IsValid() const { return EntryIndex != INDEX_NONE; }

	/**
	 * Index of the inventory entry in the inventory list
	 * Defaults to INDEX_NONE (-1) if the handle is invalid
	 */
	UPROPERTY(BlueprintReadOnly)
	int32 EntryIndex = INDEX_NONE;

	/**
	 * The item instance associated with this inventory entry
	 * Contains the actual item data and properties
	 */
	UPROPERTY(BlueprintReadOnly)
	UItemInstance* ItemInstance;

	/**
	 * Current number of items in this stack
	 * Represents how many items of this type are grouped together
	 */
	UPROPERTY(BlueprintReadOnly)
	int32 StackCount;
};
