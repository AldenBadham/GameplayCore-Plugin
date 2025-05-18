// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/EquipmentActor.h"

AEquipmentActor::AEquipmentActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
}
