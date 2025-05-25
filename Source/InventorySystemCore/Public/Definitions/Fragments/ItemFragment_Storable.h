// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "ItemFragment.h"
#include "ItemFragment_Storable.generated.h"

/**
 * @enum EItemStorageFlags
 * @brief Enum defining various storage flags for items. These flags can be combined to describe the storage properties of an item.
 */
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsPinnedByValue = true))
enum class EItemStorageFlags : uint8
{
	None = 0 UMETA(ToolTip = "No special storage properties.", Hidden),
	Unique = 1 UMETA(ToolTip = "This item should be unique in the inventory. For stacks, only one stack can exist."),
	PersistentOnDeath = 2 UMETA(ToolTip = "The item can't be lost on player death")
};

ENUM_CLASS_FLAGS(EItemStorageFlags);

/**
 * @class UItemFragment_Storable
 * @see UItemFragment
 * @brief UItemFragment_Storable defines storage properties for an item. This fragment can be attached to item definitions to specify how they behave in inventory systems.
 */
UCLASS(DisplayName = "Storable Fragment")
class INVENTORYSYSTEMCORE_API UItemFragment_Storable : public UItemFragment
{
	GENERATED_BODY()

public:
	/** Visual representation of the item in the UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FSlateBrush UIBrush;

	/** Determines how many items can be grouped in a single inventory slot. The minimum value is 1 for unstackable items */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage", meta = (ClampMin = 1))
	int MaxStackCount = 1;

	/** Individual item weight in kilograms. Used for inventory weight calculations. Minimum value is 0 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage", meta = (ClampMin = 0, Units = "kg"))
	float Weight = 0.f;

	/** Combination of EItemStorageFlags that define special storage behaviors like uniqueness or persistence */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage", meta = (Bitmask, BitmaskEnum = "/Script/InventorySystemCore.EItemStorageFlags"))
	int32 StorageFlags;

	/**
	 * Checks if this item can be stacked with others of the same type
	 * @return True if the item can be stacked (MaxStackCount > 1), false otherwise
	 */
	UFUNCTION(BlueprintCallable)
	bool CanStack() const;

	/**
	 * Checks if this item is marked as unique in storage
	 * @return True if the item has the Unique storage flag set
	 */
	UFUNCTION(BlueprintCallable)
	bool IsUnique() const;
};
