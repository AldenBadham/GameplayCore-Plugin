// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquipmentActor.generated.h"

UCLASS()
class EQUIPMENTSYSTEMCORE_API AEquipmentActor : public AActor
{
	GENERATED_BODY()

public:
	AEquipmentActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Get the owning pawn of this equipment.
	 * @return The owning pawn if it exists, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	virtual APawn* GetPawn();
};