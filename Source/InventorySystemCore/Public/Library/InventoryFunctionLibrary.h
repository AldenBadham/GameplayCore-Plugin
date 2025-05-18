// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryFunctionLibrary.generated.h"

class UItemFragment;
class UItemDefinition;
/**
 *
 */
UCLASS()
class INVENTORYSYSTEMCORE_API UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = FragmentClass))
	static const UItemFragment* FindItemDefinitionFragment(TSubclassOf<UItemDefinition> ItemDef, TSubclassOf<UItemFragment> FragmentClass);
};
