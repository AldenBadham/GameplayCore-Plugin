// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "NativeGameplayTags.h"

namespace AbilitySystemCoreTags
{
	ABILITYSYSTEMCORE_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that the plugin will use
	ABILITYSYSTEMCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_InputTag);

	ABILITYSYSTEMCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityTag);

	ABILITYSYSTEMCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_AbilityTag_DeathPersistent);
	// Gameplay tags
	ABILITYSYSTEMCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilitiesBlocked);
} // namespace AbilitySystemCoreTags
