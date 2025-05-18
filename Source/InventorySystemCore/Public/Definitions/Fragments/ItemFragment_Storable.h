// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemFragment.h"
#include "ItemFragment_Storable.generated.h"

/**
 * @enum EItemStorageFlags
 * @brief Enum defining various storage flags for items. These flags can be combined to describe the storage properties of an item.
 */
UENUM(BlueprintType)
enum class EItemStorageFlags : uint8
{
	None = 0 UMETA(Hidden), // No special storage properties
	Unique = 1,				// Item is unique
	PersistentOnDeath = 2	// Item is not lost on player death
};

ENUM_CLASS_FLAGS(EItemStorageFlags);

/**
 * @class UItemFragment_Storable
 * @see UItemFragment
 * @brief UItemFragment_Storable defines storage properties for an item. This fragment can be attached to item definitions to specify how they behave in inventory systems.
 */
UCLASS(DisplayName = "Storable")
class INVENTORYSYSTEMCORE_API UItemFragment_Storable : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	FSlateBrush UIBrush;

	/** Maximum number of items that can be stacked together */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage", meta = (ClampMin = 1))
	int MaxStackCount = 1;

	/** Weight of a single item in the appropriate unit (e.g., kilograms) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage", meta = (ClampMin = 0, Units = "kg"))
	float Weight = 0.f;

	/** Flags defining various storage properties of the item */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Storage", meta = (Bitmask, BitmaskEnum = "/Script/InventorySystemCore.EItemStorageFlags"))
	int32 StorageFlags;

	UFUNCTION(BlueprintCallable)
	bool CanStack() const;

	UFUNCTION(BlueprintCallable)
	bool IsUnique() const;
};
