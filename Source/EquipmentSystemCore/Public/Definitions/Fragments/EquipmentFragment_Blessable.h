// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "CoreMinimal.h"
#include "EquipmentFragment.h"
#include "EquipmentFragment_Blessable.generated.h"

/**
 *
 */
UCLASS(DisplayName = "Blessable")
class EQUIPMENTSYSTEMCORE_API UEquipmentFragment_Blessable : public UEquipmentFragment
{
	GENERATED_BODY()

public:
	// Abilities add to the player when the equipment is equipped and channeled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blessable")
	TArray<TSubclassOf<UGameplayAbility>> Abilities = {};

	// Abilities add to the player when the equipment is equipped and channeled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blessable")
	TArray<TSubclassOf<UGameplayEffect>> Effects = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blessable", meta = (ClampMin = 1))
	bool bBlessedOnGive = false;

	virtual void OnInstanceCreated(UEquipmentInstance* Instance) const override;
};
