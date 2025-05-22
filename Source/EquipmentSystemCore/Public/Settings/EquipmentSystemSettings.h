// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EquipmentSystemSettings.generated.h"

struct FGameplayTag;

/**
 *
 */
UCLASS(config = "Game", defaultconfig, meta = (DisplayName = "Equipment System Settings"))
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

	UPROPERTY(EditAnywhere, Category = "Equipment Slots")
	bool bRestrictSlots = true;
	
	UPROPERTY(EditAnywhere, Category = "Equipment Slots", meta = (EditCondition = "bRestrictSlots", EditConditionHides = "true"))
	TArray<FGameplayTag> SlotTemplates;
};