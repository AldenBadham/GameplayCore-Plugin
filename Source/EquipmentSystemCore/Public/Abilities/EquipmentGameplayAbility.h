// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Abilities/GameplayAbilityBase.h"
#include "CoreMinimal.h"
#include "Instances/ItemInstance.h"

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
	UEquipmentInstance* GetSourceEquipmentInstance() const;

	UFUNCTION(BlueprintCallable, Category = "Ability", BlueprintPure = false, meta = (DeterminesOutputType = InstanceClass))
	UEquipmentInstance* GetTypedSourceEquipmentInstance(const TSubclassOf<UEquipmentInstance>& InstanceClass) const;


	/**
	 * Gets the source item instance for this ability.
	 * @return The UItemInstance associated with this ability's equipment.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	UItemInstance* GetSourceItemInstance() const;

	UFUNCTION(BlueprintCallable, Category = "Ability", BlueprintPure = false, meta = (DeterminesOutputType = InstanceClass))
	UItemInstance* GetTypedSourceItemInstance(const TSubclassOf<UItemInstance>& InstanceClass) const;

	/** Template method that attempts to get the source object as the specified type.
	 * @tparam T The type to cast the source object to.
	 * @return The source object cast to the specified type, or nullptr if invalid.
	 */
	template <class T> T* TryGetSourceObject() const;

	/** Template method that retrieves the source item instance as the specified type.
	 * @tparam T The type to cast the item instance to.
	 * @return The source item instance cast to the specified type, or nullptr if invalid.
	 */
	template <class T> T* GetTypedSourceItemInstance();

	/** Template method that retrieves the source equipment instance as the specified type.
	 * @tparam T The type to cast the equipment instance to.
	 * @return The source equipment instance cast to the specified type, or nullptr if invalid.
	 */
	template <class T> T* GetTypedSourceEquipmentInstance();

	#if WITH_EDITOR
	/**
	 * Validates the data for this ability in the editor.
	 * @param Context The data validation context.
	 * @return The result of the data validation.
	 */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	#endif
};

template <class T> T* UEquipmentGameplayAbility::TryGetSourceObject() const
{
	if (UObject* const SourceObject = GetCurrentSourceObject(); IsValid(SourceObject))
	{
		return Cast<T>(SourceObject);
	}
	return nullptr;
}

template <class T> T* UEquipmentGameplayAbility::GetTypedSourceItemInstance()
{
	if (UItemInstance* Instance = GetSourceItemInstance(); IsValid(Instance))
	{
		return Cast<T>(Instance);
	}
	return nullptr;
}

template <class T> T* UEquipmentGameplayAbility::GetTypedSourceEquipmentInstance()
{
	return TryGetSourceObject<T>();
}