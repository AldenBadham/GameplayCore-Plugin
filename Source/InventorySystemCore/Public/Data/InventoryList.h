// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryChangeData.h"
#include "InventoryEntryHandle.h"
#include "InventoryEntry.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryList.generated.h"

class UItemDefinition;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/**
 * @class FInventoryList
 * @see FFastArraySerializer
 * @brief Manages the list of items in the inventory.
 *
 * This class handles the addition, removal, and management of item instances
 * in the inventory. It uses FFastArraySerializer for efficient replication.
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
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	// void PostReplicatedReceive(const FFastArraySerializer::FPostReplicatedReceiveParameters& Parameters);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInventoryEntry, FInventoryList>(Entries, DeltaParams, *this);
	}

	// ~FFastArraySerializer

	UItemInstance* Add(const TSubclassOf<UItemDefinition>& DefinitionClass, int32 Count = 1);
	void Remove(UItemInstance* Instance);

	FInventoryEntryHandle FindHandleOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	TArray<FInventoryEntryHandle> GetHandlesOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	TArray<FInventoryEntryHandle> GetAllHandles() const;

	FInventoryEntry* FindEntryOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	static TArray<FInventoryEntry*> GetEntriesOfType(const TSubclassOf<UItemDefinition>& ItemDefinition);
	static TArray<FInventoryEntry*> GetAllEntries();

protected:
	UPROPERTY(NotReplicated)
	TObjectPtr<UInventorySystemComponent> OwnerComponent;

	UPROPERTY()
	TArray<FInventoryEntry> Entries;

	void Internal_OnEntryChanged(int32 Index, const FInventoryEntry& Entry) const;
	void Internal_OnEntryAdded(int32 Index, const FInventoryEntry& Entry) const;
	void Internal_OnEntryRemoved(int32 Index, const FInventoryEntry& Entry) const;
};

// Required to specify that this structure use a NetDeltaSerializer method to help serialization operation decision
template <>
struct TStructOpsTypeTraits<FInventoryList> : TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
