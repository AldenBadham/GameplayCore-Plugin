// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/EquipmentActorSet.h"
#include "UObject/Object.h"

#include "EquipmentDefinition.generated.h"

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
	template <typename T>
	const T* FindFragmentByClass() const
	{
		return Cast<T>(FindFragmentByClass(T::StaticClass()));
	}

	virtual bool CanEquip(UEquipmentSystemComponent* EquipmentSystemComponent);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
	bool K2_CanEquip(UEquipmentSystemComponent* EquipmentSystemComponent);
	
	
	/** Display name of the equipment (used by UI) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
	FText DisplayName;

	/** Instance class to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<UEquipmentInstance> InstanceClass;
	
	/** Actors to spawn on the pawn when this is equipped */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	TArray<FEquipmentActorSet> ActorsToSpawn;

	/** Ability sets granted by this equipment */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay")
	TArray<TObjectPtr<const UAbilitySet>> AbilitySets;

	/** Equipment fragments for additional functionality */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments", Instanced)
	TArray<TObjectPtr<UEquipmentFragment>> Fragments;

};
