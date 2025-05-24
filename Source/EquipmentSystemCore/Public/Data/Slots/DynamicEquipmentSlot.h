// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "DynamicEquipmentSlot.generated.h"

class UEquipmentInstance;
class UItemInstance;

USTRUCT(BlueprintType)
struct FDynamicEquipmentSlot
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag SlotTag;

	UPROPERTY()
	TObjectPtr<UItemInstance> ItemInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UEquipmentInstance> EquipmentInstance = nullptr;
};
