// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryEntry.generated.h"

class UItemInstance;
/**
 *
 */
USTRUCT(BlueprintType)
struct INVENTORYSYSTEMCORE_API FInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend class UInventorySystemComponent;
	friend struct FInventoryList;

	FInventoryEntry()
	{
	};

	~FInventoryEntry()
	{
	};


	// FFastArraySerializer
	// Functions not virtual in FFAstArraySerializer because called by template
	void PreReplicatedRemove(const FInventoryList& InArraySerializer) const;
	void PostReplicatedAdd(const FInventoryList& InArraySerializer) const;
	// void PostReplicatedChange(const FInventoryList& InArraySerializer);
	FString GetDebugString() const;
	// ~FFastArraySerializer

private:
	UPROPERTY()
	TObjectPtr<UItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	// Used for instant local change
	UPROPERTY(NotReplicated)
	int32 LastStackCount = INDEX_NONE;
};
