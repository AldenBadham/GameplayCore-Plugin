// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemFragment.h"

#include "ItemFragment_Consumable.generated.h"

class UGameplayEffect;
/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType, DisplayName = "Consumable")
class INVENTORYSYSTEMCORE_API UItemFragment_Consumable : public UItemFragment
{
	GENERATED_BODY()

public:

	virtual void OnInstanceCreated(UItemInstance* Instance) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	TArray<TSubclassOf<UGameplayEffect>> Effects = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (ClampMin = 1))
	int32 MaxUsesCount = 1;
};
