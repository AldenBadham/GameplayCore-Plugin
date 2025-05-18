// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentComponent.h"
#include "EquipmentComponent_Blessable.generated.h"

/**
 * @class UEquipmentComponent_Blessable
 * @brief Component for blessable equipment, adding usage-related functionality.
 * This component adds properties and behaviors specific to blessable equipments,
 * such as tracking current usage.
 */
UCLASS()
class EQUIPMENTSYSTEMCORE_API UEquipmentComponent_Blessable : public UEquipmentComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	bool bIsBlessed = false;

	// UObject
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// ~UObject
};
