// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InventoryChangeData.generated.h"

class UItemInstance;


UENUM(BlueprintType)
enum class EInventoryChangeType : uint8
{
	Added,
	Removed,
	Modified
};

/**
 * @struct FInventoryChangeData
 * 
 * @brief A data payload when an item change in the inventory */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryChangeData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Index = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly)
	EInventoryChangeType ChangeType = EInventoryChangeType::Added;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 OldCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 NewCount = 0;

	bool IsValid() const { return Index != INDEX_NONE; }
};
