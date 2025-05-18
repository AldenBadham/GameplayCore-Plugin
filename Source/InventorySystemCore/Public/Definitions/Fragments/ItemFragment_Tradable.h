// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemFragment.h"
#include "ItemFragment_Tradable.generated.h"

/**
 *
 */
UCLASS(DisplayName = "Tradable")
class INVENTORYSYSTEMCORE_API UItemFragment_Tradable : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trade")
	FDataTableRowHandle ValueRowHandle;
};
