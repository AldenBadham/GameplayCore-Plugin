// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "InventorySet.generated.h"

struct FInventorySet_ItemSet;
class UItemDefinition;
class UInventorySystemComponent;
/**
 * 
 */
UCLASS()
class INVENTORYSYSTEMCORE_API UInventorySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	/**
	* Give the items set to the specified inventory system component.
	 */
	void GiveToInventorySystem(UInventorySystemComponent* InventorySystemComp);

protected:
	/** Gameplay abilities to grant when this ability set is given. */
	UPROPERTY(EditDefaultsOnly, Category = "Items", meta = (TitleProperty = "[{Quantity}] {ItemDefinition}"))
	TArray<FInventorySet_ItemSet> Items;
};
