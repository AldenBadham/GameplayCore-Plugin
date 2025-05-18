// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InventoryEntryHandle.generated.h"

class UItemInstance;
/**
 *
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryEntryHandle
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 EntryIndex = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly)
	UItemInstance* ItemInstance;

	UPROPERTY(BlueprintReadOnly)
	int32 StackCount;

	FInventoryEntryHandle() : EntryIndex(INDEX_NONE), ItemInstance(nullptr), StackCount(0)
	{
	}

	FInventoryEntryHandle(const int32 InIndex, UItemInstance* InInstance, const int32 InStackCount)
		: EntryIndex(InIndex), ItemInstance(InInstance), StackCount(InStackCount)
	{
	}

	bool IsValid() const { return EntryIndex != INDEX_NONE; }
};
