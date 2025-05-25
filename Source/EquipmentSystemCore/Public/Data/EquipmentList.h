// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/EquipmentDefinition.h"
#include "EquipmentEntry.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "EquipmentList.generated.h"

class UItemInstance;
class UAbilitySystemComponent;

struct FNetDeltaSerializeInfo;
struct FReplicationFlags;


USTRUCT(BlueprintType)
struct FEquipmentResult
{
	GENERATED_BODY();

public:
	
	UPROPERTY(BlueprintReadOnly)
	UEquipmentInstance* Instance = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag FailureReason;

	bool Succeeded() const { return !FailureReason.IsValid(); };
};


/**
 * @struct FEquipmentList
 * @see FFastArraySerializer
 * @brief Represents a list of equipment items that can be efficiently replicated using FastArraySerializer.
 */
USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	friend class UEquipmentSystemComponent;
	friend FEquipmentEntry;

	FEquipmentList();
	FEquipmentList(UEquipmentSystemComponent* InOwnerComponent);
	~FEquipmentList();

	// FFastArraySerializer
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	/** Implements network delta serialization for the equipment list. */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms) { return FastArrayDeltaSerialize<FEquipmentEntry, FEquipmentList>(Entries, DeltaParms, *this); }
	// ~FFastArraySerializer

	/** Add a new equipment instance to the list.
	 * @param DefinitionClass Definition of the type of equipment to add.
	 * @param SourceItemInstance
	 * @param OutFailureReason
	 * @return Pointer to entry added */
	FEquipmentResult Add(const TSubclassOf<UEquipmentDefinition>& DefinitionClass, UItemInstance* SourceItemInstance = nullptr);

	/**
	 * Remove the specified equipment instance from the list.
	 * @param Instance The instance to remove.
	 * @param OutFailureReason
	 */
	void Remove(UEquipmentInstance* Instance, FGameplayTag& OutFailureReason);

protected:
	
	/**
	 * Called when an entry is changed.
	 * @param Index The index of the changed entry.
	 * @param Entry The changed entry.
	 */
	void Internal_OnEntryChanged(int32 Index, const FEquipmentEntry& Entry) const;
	/**
	 * Called when an entry is added.
	 * @param Index The index of the added entry.
	 * @param Entry The added entry.
	 */
	void Internal_OnEntryAdded(int32 Index, const FEquipmentEntry& Entry) const;
	/**
	 * Called when an entry is removed.
	 * @param Index The index of the removed entry.
	 * @param Entry The removed entry.
	 */
	void Internal_OnEntryRemoved(int32 Index, const FEquipmentEntry& Entry) const;

	/** Get the ability system component of the owner. */
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:
	
	/** Array of equipment entries */
	UPROPERTY()
	TArray<FEquipmentEntry> Entries;

	/** Owner component of this equipment list */
	UPROPERTY(NotReplicated)
	TObjectPtr<UEquipmentSystemComponent> OwnerComponent = nullptr;
};

// TStructOpsTypeTraits specialization to enable NetDeltaSerializer for FEquipmentList
template <>
struct TStructOpsTypeTraits<FEquipmentList> : TStructOpsTypeTraitsBase2<FEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
