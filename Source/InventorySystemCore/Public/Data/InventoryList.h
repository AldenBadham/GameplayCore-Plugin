// Copyright (C) [Year] [Company Name]. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "InventoryChangeData.h"
#include "InventoryEntry.h"
#include "InventoryEntryHandle.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryList.generated.h"

struct FGameplayTag;
class UItemDefinition;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;



USTRUCT(BlueprintType)
struct FInventoryAddResult
{
	GENERATED_BODY();

public:
	
	UPROPERTY(BlueprintReadOnly)
	TArray<UItemInstance*> Instances;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag FailureReason;

	bool Succeeded() const { return FailureReason.IsValid() == false; };
};


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
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	/** Implements network delta serialization for the equipment list. */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams) { return FastArrayDeltaSerialize<FInventoryEntry, FInventoryList>(Entries, DeltaParams, *this); }
	// ~FFastArraySerializer

	/**
	 * Adds an item to the inventory list from its definition.
	 * @param DefinitionClass The item definition class to add.
	 * @param Count The number of items to add.
	 * @return Item instances if added successfully, otherwise returns an empty array.
	 */
	FInventoryAddResult Add(const TSubclassOf<UItemDefinition>& DefinitionClass, int32 Count = 1);
	/**
	 * Removes an item from the inventory.
	 * @param Instance The item instance to remove.
	 * @param OutFailureReason
	 */
	void Remove(UItemInstance* Instance, FGameplayTag& OutFailureReason);

	FInventoryAddResult AddItemInstance(UItemInstance* ItemInstance, int32 Count = 1);

	void AddItemInstance(const FInventoryEntry& Entry, FGameplayTag& OutFailureReason);

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

	UItemInstance* CreateItemInstance(const TSubclassOf<UItemDefinition>& DefinitionClass, int32& Count);

	/**
	 * Checks if an item of the specified definition can be added
	 * @param DefinitionClass The item definition class to check
	 * @param OutFailureReason
	 * @param InCount
	 * @return True if the item can be added, false otherwise
	 */
	bool CanAdd(const TSubclassOf<UItemDefinition>& DefinitionClass, FGameplayTag& OutFailureReason, int32 InCount = 1);

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
	
protected:

	/** Array of inventory entries managed by this list */
	UPROPERTY()
	TArray<FInventoryEntry> Entries;
	
	/** The inventory system component that owns this list. Not replicated */
	UPROPERTY(NotReplicated)
	TObjectPtr<UInventorySystemComponent> OwnerComponent = nullptr;

};

// Required to specify that this structure uses a NetDeltaSerializer method to help serialization operation decision
template <>
struct TStructOpsTypeTraits<FInventoryList> : TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
