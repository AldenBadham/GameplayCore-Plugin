// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryFunctionLibrary.generated.h"

class UItemFragment;
class UItemDefinition;


/**
 * @class UInventoryFunctionLibrary
 * @see UBlueprintFunctionLibrary
 * @brief Function library containing utility functions for the Inventory System.
 * @details Provides helper functions to interact with item definitions and their fragments.
 */
UCLASS()
class INVENTORYSYSTEMCORE_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Finds a specific fragment in an item definition
	 * @param ItemDef The item definition class to search in
	 * @param FragmentClass The type of fragment to find
	 * @return The found fragment instance, or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = FragmentClass))
	static const UItemFragment* FindItemDefinitionFragment(TSubclassOf<UItemDefinition> ItemDef, TSubclassOf<UItemFragment> FragmentClass);
};
