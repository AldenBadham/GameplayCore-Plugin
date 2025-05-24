// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Data/EquipmentActorSet.h"
#include "UObject/Object.h"

#include "EquipmentDefinition.generated.h"

class USlotPolicy;
class UEquipmentSystemComponent;
class UEquipmentInstance;
class UAbilitySet;
class UEquipmentFragment;

/**
 * @class UEquipmentDefinition
 * @see UObject
 * @brief Data asset definition of an item that can be equipped by the pawn.
 * And what to do to the pawn on which it is equipped
 */
UCLASS(Blueprintable, Abstract, BlueprintType)
class EQUIPMENTSYSTEMCORE_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

	friend struct FEquipmentList;
	friend class UEquipmentSystemComponent;

public:
	UEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
	// ~UObject

	/**
	 * Try to find fragment of class FragmentClass in this equipment definition
	 * @param FragmentClass Class of the EquipmentFragment to search
	 * @return Pointer to the found fragment, or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UEquipmentFragment* FindFragmentByClass(TSubclassOf<UEquipmentFragment> FragmentClass) const;

	/**
	 * Template function to find fragment of class T in this equipment definition
	 * @return Pointer to the found fragment of type T, or nullptr if not found
	 */
	template <typename T> const T* FindFragmentByClass() const { return Cast<T>(FindFragmentByClass(T::StaticClass())); }

	virtual bool CanBeEquipped(UEquipmentSystemComponent* EquipmentSystemComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
	bool K2_CanBeEquipped(UEquipmentSystemComponent* EquipmentSystemComponent);

	/**
	 * Get the display name of this equipment
	 * @return Display name as FText
	 */
	FText GetDisplayName() const { return DisplayName; }

protected:
	/** Instance class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Definition")
	TSubclassOf<UEquipmentInstance> InstanceClass;

	/** Display name of the equipment (used by UI) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot")
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Slot")
	TObjectPtr<const USlotPolicy> SlotPolicy;

	/** Actors to spawn on the pawn when this is equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TArray<FEquipmentActorSet> ActorsToSpawn;

	/** Ability sets granted by this equipment */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TArray<TObjectPtr<const UAbilitySet>> AbilitySets;

	/** Equipment fragments for additional functionality */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments", Instanced)
	TArray<TObjectPtr<UEquipmentFragment>> Fragments;
};