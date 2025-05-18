// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/EquipmentDefinition.h"
#include "EquipmentEntry.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "EquipmentList.generated.h"

class UAbilitySystemComponent;

struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

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
	// void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	/** Implements network delta serialization for the equipment list. */
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms) { return FastArrayDeltaSerialize<FEquipmentEntry, FEquipmentList>(Entries, DeltaParms, *this); }

	// ~FFastArraySerializer

	/** Add a new equipment instance to the list.
	 * @param DefinitionClass Definition of the type of equipment to add.
	 * @param Count Number of instances to add (default is 1).
	 * @return Pointer to entry added */
	UEquipmentInstance* Add(const TSubclassOf<UEquipmentDefinition>& DefinitionClass, int32 Count = 1);

	/**
	 * Remove the specified equipment instance from the list.
	 * @param Instance The instance to remove.
	 */
	void Remove(UEquipmentInstance* Instance);

private:
	/** Get the ability system component of the owner. */
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

	/** Array of equipment entries */
	UPROPERTY()
	TArray<FEquipmentEntry> Entries;

	/** Owner component of this equipment list */
	UPROPERTY(NotReplicated)
	TObjectPtr<UEquipmentSystemComponent> OwnerComponent = nullptr;
};

// TStructOpsTypeTraits specialization to enable NetDeltaSerializer for FEquipmentList
template <> struct TStructOpsTypeTraits<FEquipmentList> : TStructOpsTypeTraitsBase2<FEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
