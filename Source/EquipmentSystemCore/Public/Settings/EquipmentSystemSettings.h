// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "EquipmentSystemSettings.generated.h"

struct FGameplayTag;

#define GET_EQUIPMENT_SETTINGS_VAR(a) (GetDefault<UEquipmentSystemSettings>()->a)

namespace EquipmentSystemVariables
{
	static bool bDrawWeaponColliders = true;
	inline FAutoConsoleVariableRef CVarDrawWeaponColliders{
		TEXT("Gameplay.Collision.DrawWeaponColliders"),
		bDrawWeaponColliders,
		TEXT("True to enable weapon colliders drawing (default = false).")};

	
	static float SweepTraceTickRate = 1.0f / 30.0f;
	inline FAutoConsoleVariableRef CVarSweepTraceTickRate{
		TEXT("Gameplay.Collision.SweepTraceTickRate"),
		SweepTraceTickRate,
		TEXT("Tick rate of the sweep trace collision test (default = 1/30 sec).")};

	
	static bool bDrawSweepTraces = true;
	inline FAutoConsoleVariableRef CVarDrawSweepTraces{
		TEXT("Gameplay.Collision.DrawSweepTraces"),
		bDrawSweepTraces,
		TEXT("True to enable sweep shape trace drawing (default = false).")};
	
	static float SweepTracesDuration = 1.0f;
	inline FAutoConsoleVariableRef CVarDrawSweepTracesDuration{
		TEXT("Gameplay.Collision.SweepTracesDuration"),
		SweepTracesDuration,
		TEXT("Lifetime of the sweep shape trace drawing (default = 1.0 sec).")};
	
	static float SweepTracesLineThickness = 0.2f;
	inline FAutoConsoleVariableRef CVarSweepTracesLineThickness{
		TEXT("Gameplay.Collision.SweepTracesLineThickness"),
		SweepTracesLineThickness,
		TEXT("Collision debug trace line thickness (default = 0.2).")};

	
	static bool bDrawSweepTracesHitNormals = true;
	inline FAutoConsoleVariableRef CVarDrawSweepTracesHitNormals{
		TEXT("Gameplay.Collision.DrawSweepTracesHitNormals"),
		bDrawSweepTraces,
		TEXT("True to enable sweep hit normals trace drawing (default = false).")};
	
	static float SweepTracesHitNormalsDuration = 1.0f;
	inline FAutoConsoleVariableRef CVarSweepTracesHitNormalsDuration{
		TEXT("Gameplay.Collision.SweepTracesHitNormalsDuration"),
		SweepTracesHitNormalsDuration,
		TEXT("Lifetime of the weapon sweep hit trace drawing (default = 1.0 sec).")};
	
	
} // namespace EquipmentSystemVariables


/**
 *
 */
UCLASS(Config = "Game", defaultConfig, meta = (DisplayName = "Equipment System Settings"))
class EQUIPMENTSYSTEMCORE_API UEquipmentSystemSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	// UDeveloperSettings
	virtual void PostInitProperties() override;
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	// ~UDeveloperSettings

	/** Accessor and initializer */
	UFUNCTION(BlueprintPure, Category = "Settings", meta=(DisplayName="Get Equipment System Settings"))
	static UEquipmentSystemSettings* GetEquipmentSystemSettings();

	
public:
	
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons|Sweep Collision", meta = (DisplayName = "Weapon sweep channel"))
	TEnumAsByte<ECollisionChannel> SweepChannel = ECC_PhysicsBody;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display", meta = (ConsoleVariable = "Gameplay.Collision.DrawWeaponColliders"))
	bool bDrawWeaponColliders = true;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display", meta = (ConsoleVariable = "Gameplay.Collision.SweepTraceTickRate"))
	float SweepTraceTickRate = 1.0f / 30.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display", meta = (ConsoleVariable = "Gameplay.Collision.DrawSweepTraces"))
	bool bDrawSweepTraces = true;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display", meta = (ConsoleVariable = "Gameplay.Collision.SweepTracesDuration"))
	float SweepTracesDuration = 1.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display", meta = (ConsoleVariable = "Gameplay.Collision.SweepTracesLineThickness"))
	float SweepTracesLineThickness = 0.2f;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display")
	FColor SweepTracesColor = FColor::Red;
	

	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display", meta = (ConsoleVariable = "Gameplay.Collision.DrawSweepTracesHitNormals"))
	bool bDrawSweepTracesHitNormals = true;

	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display", meta = (ConsoleVariable = "Gameplay.Collision.SweepTracesHitNormalsDuration"))
	float SweepTracesHitNormalsDuration = 1.0f;
	
	UPROPERTY(Config, EditAnywhere, Category = "Weapons|Debug display")
	FColor SweepTracesHitNormalsColor = FColor::Green;
	
private:
	// UObject
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// ~UObject
	
	
};
