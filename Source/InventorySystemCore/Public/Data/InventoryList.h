// Copyright (C) [Year] [Company Name]. All Rights Reserved.

#pragma once

#include "InventoryChangeData.h"
#include "InventoryEntry.h"
#include "InventoryEntryHandle.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryList.generated.h"

class UItemDefinition;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/**
 * @class FInventoryList
 * @see FFastArraySerializer
 * @brief A replicated container that manages a list of inventory entries
 * @details Handles the storage, replication, and management of inventory entries including
 * addition, removal, and modification of items. Utilizes FastArraySerializer for efficient
 * network replication and provides interfaces for item manipulation and querying.
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	friend class UInventorySystemComponent;
	friend FInventoryEntry;

	FInventoryList();
	FInventoryList(UInventorySystemComponent* InOwnerComponent);

	// FFastArraySerializer
	// Functions not virtual in FFAstArraySerializer because called by template
	/**
	 * Called before entries are removed during replication
	 * @param RemovedIndices Indices of entries being removed
	 * @param FinalSize Final size of the array after removal
	 */
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);

	/**
	 * Called after entries are added during replication
	 * @param AddedIndices Indices of newly added entries
	 * @param FinalSize Final size of the array after addition
	 */
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);

	/**
	 * Called after entries are changed during replication
	 * @param ChangedIndices Indices of modified entries
	 * @param FinalSize Current size of the array
	 */
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	// void PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams) { return FastArrayDeltaSerialize<FInventoryEntry, FInventoryList>(Entries, DeltaParams, *this); }
	// ~FFastArraySerializer

	/**
	 * Adds an item to the inventory list from its definition.
	 * @param DefinitionClass The item definition class to add.
	 * @param Count The number of items to add.
	 * @return Item instances if added successfully, otherwise returns an empty array.
	 */
	TArray<UItemInstance*> Add(const TSubclassOf<UItemDefinition>& DefinitionClass, int32 Count = 1);
	/**
	 * Removes an item from the inventory.
	 * @param Instance The item instance to remove.
	 */
	void Remove(UItemInstance* Instance);

	void AddItemInstance(UItemInstance* ItemInstance, int32 Count = 1);

	void AddItemInstance(const FInventoryEntry& Entry);

	/**
	 * Finds the handle of the first entry of a specific item definition.
	 * @param ItemDefinition The item definition class to find.
	 * @return The inventory entry handle if found, otherwise an invalid handle.
	 */
	FInventoryEntryHandle FindHandleOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	/**
	 * Gets all handles of a specific item definition.
	 * @param ItemDefinition The item definition class to find.
	 * @return An array of inventory entry handles.
	 */
	TArray<FInventoryEntryHandle> GetHandlesOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	/**
	 * Gets all handles in the inventory.
	 * @return An array of all inventory entry handles.
	 */
	TArray<FInventoryEntryHandle> GetAllHandles() const;

	/**
	 * Finds the first entry of a specific item definition.
	 * @param ItemDefinition The item definition class to find.
	 * @return The inventory entry if found, otherwise nullptr.
	 */
	FInventoryEntry* FindEntryOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);

	/**
	 * Gets all entries matching the specified item definition
	 * @param ItemDefinition The item definition class to find
	 * @return Array of inventory entries matching the definition
	 * @see FInventoryEntry
	 */
	static TArray<FInventoryEntry*> GetEntriesOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	/**
	 * Gets all entries currently in the inventory
	 * @return Array containing all inventory entries
	 * @see FInventoryEntry
	 */
	static TArray<FInventoryEntry*> GetAllEntries();

protected:
	/** The inventory system component that owns this list. Not replicated */
	UPROPERTY(NotReplicated)
	TObjectPtr<UInventorySystemComponent> OwnerComponent;

	/** Array of inventory entries managed by this list */
	UPROPERTY()
	TArray<FInventoryEntry> Entries;

	UItemInstance* CreateItemInstance(const TSubclassOf<UItemDefinition>& DefinitionClass, int32& Count);

	/**
	 * Checks if an item of the specified definition can be added
	 * @param DefinitionClass The item definition class to check
	 * @param CheckUniqueness
	 * @return True if the item can be added, false otherwise
	 */
	bool CanAdd(const TSubclassOf<UItemDefinition>& DefinitionClass, bool CheckUniqueness = true);

	/**
	 * Called when an entry is changed.
	 * @param Index The index of the changed entry.
	 * @param Entry The changed entry.
	 */
	void Internal_OnEntryChanged(int32 Index, const FInventoryEntry& Entry) const;
	/**
	 * Called when an entry is added.
	 * @param Index The index of the added entry.
	 * @param Entry The added entry.
	 */
	void Internal_OnEntryAdded(int32 Index, const FInventoryEntry& Entry) const;
	/**
	 * Called when an entry is removed.
	 * @param Index The index of the removed entry.
	 * @param Entry The removed entry.
	 */
	void Internal_OnEntryRemoved(int32 Index, const FInventoryEntry& Entry) const;
};

// Required to specify that this structure uses a NetDeltaSerializer method to help serialization operation decision
template <> struct TStructOpsTypeTraits<FInventoryList> : TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};