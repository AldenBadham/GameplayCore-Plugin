// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemComponent.h"
#include "ItemComponent_Consumable.generated.h"

/**
 * @class UItemComponent_Consumable
 * @brief Component for consumable items, adding usage-related functionality.
 * This component adds properties and behaviors specific to consumable items,
 * such as tracking current usage.
 */
UCLASS()
class INVENTORYSYSTEMCORE_API UItemComponent_Consumable : public UItemComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	float CurrentUse = 0.0f;

	// UObject
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// ~UObject
};
