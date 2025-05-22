#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EquipmentSystemSettings.generated.h"

struct FEquipmentSlotTemplate;

// TODO : Add IDetailsCustomization class to handle it more properly. Will be enough for the moment

/**
 *
 */
UCLASS(config = Engine, defaultconfig, meta = (DisplayName = "Equipment System Settings"))
class EQUIPMENTSYSTEMCORE_API UEquipmentSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// UDeveloperSettings
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	// ~UDeveloperSettings

	/** Accessor and initializer */
	static UEquipmentSystemSettings* Get();

private:
	// UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// ~UObject
};