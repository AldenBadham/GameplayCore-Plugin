// Fill out your copyright notice in the Description page of Project Settings.

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
