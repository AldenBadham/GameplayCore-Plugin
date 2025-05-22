#pragma once

#include "CoreMinimal.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an GameplayEvent with a specific tag is triggered.
	OnGameplayEvent,

	// Try to activate the ability by tag.
	ByTag,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};