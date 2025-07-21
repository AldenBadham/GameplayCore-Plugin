// Copyright (C) [Year] [Company Name]. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "InventoryChangeData.h"
#include "InventoryEntry.h"
#include "InventoryEntryHandle.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryList.generated.h"

class UInventoryContainer;
struct FGameplayTag;
class UItemDefinition;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;


USTRUCT(BlueprintType)
struct FInventoryResult
{
	GENERATED_BODY()
	;

	UPROPERTY(BlueprintReadOnly)
	TArray<UItemInstance*> Instances;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag FailureReason;

	bool Succeeded() const { return FailureReason.IsValid() == false; };

	int32 Num() const { return Instances.Num(); }
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
	friend class UInventoryContainer;
	friend FInventoryEntry;

	FInventoryList();
	FInventoryList(UInventorySystemComponent* InOwnerComponent);

	// FFastArraySerializer
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	/** Implements network delta serialization for the equipment list. */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInventoryEntry, FInventoryList>(Entries, DeltaParams, *this);
	}

	// ~FFastArraySerializer


	FInventoryResult AddFromDefinition(const TSubclassOf<UItemDefinition>& DefinitionClass, int32 Count = 1);
	FInventoryResult AddInstance(UItemInstance* ItemInstance, int32 Count = 1);

	void RemoveInstance(UItemInstance* Instance);
	bool RemoveFromHandle(const FInventoryEntryHandle& Handle, FGameplayTag& OutFailureReason);
	bool RemoveFromIndex(int32 Index, FGameplayTag& OutFailureReason);

	FInventoryEntryHandle MakeHandle(int32 Index) const;
	FInventoryEntryHandle FindHandleFromInstance(UItemInstance* Instance) const;
	FInventoryEntryHandle FindHandleOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);

	TArray<FInventoryEntryHandle> GetAllHandles() const;
	TArray<FInventoryEntryHandle> GetHandlesOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	TArray<FInventoryEntry*> GetAllEntries();
	FInventoryEntry* FindEntryOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	TArray<FInventoryEntry*> GetEntriesOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);

	int32 GetStackCountByDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const;
	int32 GetTotalCountByDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const;

	void SetOwningComponent(UInventorySystemComponent* Component);
	void SetOwningContainer(UInventoryContainer* Container);

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

	/** Array of inventory entries managed by this list */
	UPROPERTY()
	TArray<FInventoryEntry> Entries;

	/** The inventory system component that owns this list. Not replicated */
	UPROPERTY(NotReplicated)
	TObjectPtr<UInventorySystemComponent> OwningComponent = nullptr;

	/** The inventory container that owns this list. Not replicated */
	UPROPERTY(NotReplicated)
	TObjectPtr<UInventoryContainer> OwningContainer = nullptr;
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
