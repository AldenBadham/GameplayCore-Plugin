// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SlotDefinition.generated.h"

class USlotPolicy;
/**
*
*/
USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FSlotDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slot", meta = (Categories = "Equipment.Slot"))
	FGameplayTag SlotTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	FName UIGroup;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	int32 DisplayOrder = 0;
};
