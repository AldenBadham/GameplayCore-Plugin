// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once
#include "GameplayTagContainer.h"

#include "EquipmentActorSet.generated.h"

USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FEquipmentActorSet
{
	GENERATED_BODY()

	FEquipmentActorSet()
	{
	};

	/** The class of actor to spawn and attach. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<AActor> ActorClass;

	/** The socket on the owning actor to attach the spawned actor to. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FName AttachSocket;

	/** The transform (position, rotation, scale) to apply relative to the socket. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FTransform AttachTransform;
};
