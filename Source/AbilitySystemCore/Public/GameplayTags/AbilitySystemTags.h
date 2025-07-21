// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "NativeGameplayTags.h"

namespace AbilitySystemTags
{
	ABILITYSYSTEMCORE_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	ABILITYSYSTEMCORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_DATA_EFFECT_PERSISTENT);
}
