// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "GameplayTags/AbilitySystemTags.h"

namespace AbilitySystemTags
{
	// Data Gameplay Effect tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_DATA_EFFECT_PERSISTENT, "Data.Effect.Persistent", "Indicate that the GameplayEffect should not be removed on death (so it stays when resurrected in multiplayer)");
}
