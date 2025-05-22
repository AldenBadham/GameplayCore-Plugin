// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Actors/EquipmentActor.h"

AEquipmentActor::AEquipmentActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.Get())
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
}

APawn* AEquipmentActor::GetPawn()
{
	return Cast<APawn>(GetOwner());
}