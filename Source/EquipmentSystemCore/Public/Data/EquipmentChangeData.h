// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "EquipmentChangeData.generated.h"

class UEquipmentInstance;
struct FEquipmentEntry;
class UEquipmentnstance;

/**
 * Defines the types of changes that can occur to Equipment entries
 */
UENUM(BlueprintType)
enum class EEquipmentChangeType : uint8
{
	Equipped, ///< Equipment has just been equipped
	Unequipped, ///< Equipment has just been unequipped
	Modified ///< Equipment properties were modified
};

/**
 * @struct FEquipmentChangeData
 * @see UItemInstance, FEquipmentEntry, EEquipmentChangeType
 * @brief Represents a data payload for tracking Equipment item changes including addition, removal, and modification of items
 * @details Contains information about the affected item instance, its index in the Equipment, the type of change, 
 *		  and stack count changes before and after the modification occurred. Used for notifying systems about
 *		  Equipment state changes and maintaining synchronization.
 */
USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FEquipmentChangeData
{
	GENERATED_BODY()

	FEquipmentChangeData()
	{
	}

	/**
	 * Constructs an Equipment change data instance with the specified parameters
	 * @param InIndex The index of the Equipment entry that was modified
	 * @param Entry The Equipment entry containing the item instance and stack count
	 * @param InChangeType The type of modification that occurred
	 */
	FEquipmentChangeData(const int32 InIndex, const FEquipmentEntry& Entry, const EEquipmentChangeType InChangeType = EEquipmentChangeType::Equipped);

	/** 
	 * Zero-based index of the modified Equipment entry
	 * Set to INDEX_NONE (-1) when the entry is invalid
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	int32 Index = INDEX_NONE;

	/** 
	 * Reference to the new equipment instance
	 * Contains the equipment's definition and properties
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UEquipmentInstance> NewInstance = nullptr;

	/** 
	 * Reference to the previous equipment instance
	 * Contains the equipment's definition and properties
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UEquipmentInstance> OldInstance = nullptr;

	/** 
	 * Specifies whether the item was added, removed, or modified
	 * Defaults to Added when not specified
	 */
	UPROPERTY(BlueprintReadOnly)
	EEquipmentChangeType ChangeType = EEquipmentChangeType::Equipped;

	/**
	 * Checks if this Equipment change data is valid
	 * @return True if the index is valid (not INDEX_NONE), false otherwise
	 */
	bool IsValid() const { return Index != INDEX_NONE; }
};
