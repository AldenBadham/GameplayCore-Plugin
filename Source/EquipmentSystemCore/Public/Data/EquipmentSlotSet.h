// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"

#include "EquipmentSlotSet.generated.h"

class UEquipmentInstance;
class UItemInstance;


USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FEquipmentSlotSet
{
	GENERATED_BODY()

public:

	FEquipmentSlotSet() : SlotTag(FGameplayTag::EmptyTag) {};
	FEquipmentSlotSet(const FGameplayTag& InSlotTag) : SlotTag(InSlotTag) {};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "Equipment.Slot"))
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UItemInstance> ItemInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UEquipmentInstance> EquipmentInstance = nullptr;
};