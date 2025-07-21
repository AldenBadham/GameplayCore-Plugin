// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Settings/EquipmentSystemSettings.h"

#include "Misc/ConfigUtilities.h"

DEFINE_LOG_CATEGORY_STATIC(LogEquipmentSettings, Warning, All)

void UEquipmentSystemSettings::PostInitProperties()
{
	Super::PostInitProperties();
	
	if (IsTemplate())
	{
		// We want the .ini file to have precedence over the CVar constructor, so we apply the ini to the CVar before following the regular UDeveloperSettingsBackedByCVars flow
		UE::ConfigUtilities::ApplyCVarSettingsFromIni(TEXT("/Script/EquipmentSystemCore.EquipmentSystemSettings"), *GEngineIni, ECVF_SetByProjectSetting);
	}

	Super::PostInitProperties();
}

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
