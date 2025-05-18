// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/Components/EquipmentComponent_Blessable.h"

#include "Net/UnrealNetwork.h"

void UEquipmentComponent_Blessable::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bIsBlessed);
}
