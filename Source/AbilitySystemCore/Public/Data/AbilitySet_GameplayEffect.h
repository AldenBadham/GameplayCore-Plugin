#pragma once

#include "GameplayEffect.h"

#include "AbilitySet_GameplayEffect.generated.h"

/**
 * FAbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct FAbilitySet_GameplayEffect
{
	GENERATED_BODY()

	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};