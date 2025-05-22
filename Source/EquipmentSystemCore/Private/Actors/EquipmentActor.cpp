#include "Actors/EquipmentActor.h"

AEquipmentActor::AEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
}

AEquipmentActor::AEquipmentActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
}

APawn* AEquipmentActor::GetPawn()
{
	return Cast<APawn>(GetOwner());
}