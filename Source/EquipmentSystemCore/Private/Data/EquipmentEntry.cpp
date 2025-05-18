// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/EquipmentEntry.h"
#include "Definitions/EquipmentDefinition.h"
#include "Instances/EquipmentInstance.h"

FString FEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s [Def: %s]"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}
