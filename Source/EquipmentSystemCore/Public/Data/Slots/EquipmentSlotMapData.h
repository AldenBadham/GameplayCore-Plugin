// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "EquipmentSlotMapData.generated.h"

struct FSlotDefinition;

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class EQUIPMENTSYSTEMCORE_API UEquipmentSlotMapData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSlotDefinition> Slots;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
