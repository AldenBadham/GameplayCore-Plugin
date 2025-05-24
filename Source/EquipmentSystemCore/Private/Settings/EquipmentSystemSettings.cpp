// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Settings/EquipmentSystemSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogEquipmentSettings, Warning, All)

FName UEquipmentSystemSettings::GetContainerName() const
{
	static const FName Name("Project");
	return Name;
}

FName UEquipmentSystemSettings::GetCategoryName() const
{
	static const FName Name("Game");
	return Name;
}

UEquipmentSystemSettings* UEquipmentSystemSettings::GetEquipmentSystemSettings()
{
	// This is a singleton. Use a default object
	return GetMutableDefault<UEquipmentSystemSettings>();
}

void UEquipmentSystemSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}