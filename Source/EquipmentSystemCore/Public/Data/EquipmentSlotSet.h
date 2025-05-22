// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "GameplayTagContainer.h"

#include "EquipmentSlotSet.generated.h"

class UEquipmentInstance;
class UItemInstance;

USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FEquipmentSlotSet
{
	GENERATED_BODY()

	FEquipmentSlotSet()
		: SlotTag(FGameplayTag::EmptyTag) {};

	FEquipmentSlotSet(const FGameplayTag& InSlotTag)
		: SlotTag(InSlotTag) {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "Equipment.Slot"))
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UItemInstance> ItemInstance = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UEquipmentInstance> EquipmentInstance = nullptr;
};