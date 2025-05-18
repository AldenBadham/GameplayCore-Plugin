// Fill out your copyright notice in the Description page of Project Settings.

#include "Instances/Components/ItemComponent_Consumable.h"

#include "Net/UnrealNetwork.h"

void UItemComponent_Consumable::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentUse);
}
