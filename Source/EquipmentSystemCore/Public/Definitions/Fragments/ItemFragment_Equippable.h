// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Definitions/Fragments/ItemFragment.h"
#include "ItemFragment_Equippable.generated.h"

class UEquipmentSystemComponent;
class UEquipmentDefinition;

/**
 * 
 */
UCLASS(DisplayName = "Equipable")
class EQUIPMENTSYSTEMCORE_API UItemFragment_EquipableItem : public UItemFragment
{
	GENERATED_BODY()

public:

	virtual void OnInstanceCreated(UItemInstance* Instance) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

protected:

	UEquipmentSystemComponent* FindEquipmentComponent() const;
};
