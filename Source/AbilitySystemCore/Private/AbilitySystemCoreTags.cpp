// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "AbilitySystemCoreTags.h"

#include "Engine/EngineTypes.h"

namespace AbilitySystemCoreTags
{
	// Activation fail tags

	// Input tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_INPUT, "Input", "Root input tag (should only be used for categories or filtering)");

	// Ability Tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ABILITY, "Ability", "Root ability tag (should only be used for categories or filtering)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_ABILITY_PERSISTENTONDEATH, "Ability.PersistentOnDeath", "Tag the ability to be persistent on avatar death");

	// Gameplay tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_GAMEPLAY_ABILITIESBLOCKED, "Gameplay.AbilitiesBlocked", "Block the activation of any ability with input activation policy");
} // namespace AbilitySystemCoreTags
