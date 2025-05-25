// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryEntry.generated.h"

class UItemInstance;

/**
 * @struct FInventoryEntry
 * @see FFastArraySerializerItem, UItemInstance, FInventoryList
 * @brief Represents a single inventory entry with item instance and stack management
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend class UInventorySystemComponent;
	friend class UInventoryContainer;
	friend struct FInventoryList;
	friend struct FInventoryChangeData;
	friend struct FInventoryEntryHandle;

	FInventoryEntry()
	{
	};

	~FInventoryEntry()
	{
	};

	// FFastArraySerializer
	// Functions not virtual in FFAstArraySerializer because called by template
	void PreReplicatedRemove(const FInventoryList& InArraySerializer) const;
	void PostReplicatedAdd(const FInventoryList& InArraySerializer) const;
	void PostReplicatedChange(const FInventoryList& InArraySerializer) const;
	FString GetDebugString() const;
	// ~FFastArraySerializer

private:
	/**
	 * The actual item instance being stored in this inventory entry
	 * This instance contains the item's data and properties
	 */
	UPROPERTY()
	TObjectPtr<UItemInstance> Instance = nullptr;

	/**
	 * Current number of items in this stack
	 * Represents how many items of this type are grouped together
	 */
	UPROPERTY()
	int32 StackCount = 0;

	/** 
	 * Used to detect local stack changes without replication
	 * Helps with client-side prediction of stack modifications
	 * @note Not replicated - used for client prediction only
	 */
	UPROPERTY(NotReplicated)
	int32 LastStackCount = INDEX_NONE;
};
