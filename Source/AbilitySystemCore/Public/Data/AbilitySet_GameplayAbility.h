#pragma once

#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityBase.h"
#include "AbilitySet_GameplayAbility.generated.h"


/**
 * FAbilitySet_GameplayAbility
 *
 *	Data used by the ability set to grant gameplay abilities.
 */
USTRUCT(BlueprintType)
struct FAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbilityBase> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 Level = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};
