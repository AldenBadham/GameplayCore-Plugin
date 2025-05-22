#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "EquipmentSlotMapData.generated.h"

/**
 *
 */
UCLASS()
class EQUIPMENTSYSTEMCORE_API UEquipmentSlotMapData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (Categories = "Equipment.Slot"))
	TArray<FGameplayTag> Slots;
};