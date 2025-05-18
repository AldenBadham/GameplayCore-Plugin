// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbilityBase.h"
#include "CoreMinimal.h"
#include "EquipmentGameplayAbility.generated.h"

class UItemInstance;
class UEquipmentInstance;

/**
 * @class UEquipmentGameplayAbility
 * @see UGameplayAbilityBase
 * @brief This class represents a gameplay ability specifically for equipment.
 * It extends the base gameplay ability with equipment-specific functionality.
 */
UCLASS()
class EQUIPMENTSYSTEMCORE_API UEquipmentGameplayAbility : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UEquipmentGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Gets the source equipment instance for this ability.
	 * @return The UEquipmentInstance associated with this ability.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	UEquipmentInstance* GetSourceEquipment() const;

	/**
	 * Gets the source item instance for this ability.
	 * @return The UItemInstance associated with this ability's equipment.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	UItemInstance* GetSourceItem() const;

#if WITH_EDITOR
	/**
	 * Validates the data for this ability in the editor.
	 * @param Context The data validation context.
	 * @return The result of the data validation.
	 */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
