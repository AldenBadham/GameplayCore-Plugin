// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"

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

	static bool bDrawWeaponSweepTraces = true;
	inline FAutoConsoleVariableRef CVarDrawWeaponSweepTraces{
		TEXT("Gameplay.Collision.DrawWeaponSweepTraces"),
		bDrawWeaponSweepTraces,
		TEXT("True to enable weapon sweep shape trace drawing (default = false).")};
	
	static float WeaponSweepTracesDuration = 1.0f;
	inline FAutoConsoleVariableRef CVarDrawWeaponSweepTracesDuration{
		TEXT("Gameplay.Collision.WeaponSweepTracesDuration"),
		WeaponSweepTracesDuration,
		TEXT("Lifetime of the weapon sweep shape trace drawing (default = 1.0 sec).")};
	
	static float WeaponSweepTracesThickness = 1.0f;
	inline FAutoConsoleVariableRef CVarDrawWeaponSweepTracesThickness{
		TEXT("Gameplay.Collision.WeaponSweepTracesThickness"),
		WeaponSweepTracesThickness,
		TEXT("Weapon trace line thickness (default = 1.0).")};
	
} // namespace EquipmentSystemVariables