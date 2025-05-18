// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/EquipmentSystemSettings.h"


DEFINE_LOG_CATEGORY_STATIC(LogEquipmentSettings, Warning, All)


FName UEquipmentSystemSettings::GetContainerName() const
{
	return TEXT("Project");
}

FName UEquipmentSystemSettings::GetCategoryName() const
{
	return TEXT("Game");
}

UEquipmentSystemSettings* UEquipmentSystemSettings::Get()
{
	// this is singletone. Use default object
	return GetMutableDefault<UEquipmentSystemSettings>();
}

void UEquipmentSystemSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}