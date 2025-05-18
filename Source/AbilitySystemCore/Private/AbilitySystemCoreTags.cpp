// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemCoreTags.h"

#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"


namespace AbilitySystemCoreTags
{
	// Activation fail tags

	// Input tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InputTag, "InputTag", "Root input tag (should only be used for categories or filtering)");

	// Ability Tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_AbilityTag, "AbilityTag", "Root ability tag (should only be used for categories or filtering)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_AbilityTag_DeathPersistent, "AbilityTag.DeathPersistent", "Tag the ability to be persistent on avatar death");

	// Gameplay tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Gameplay_AbilitiesBlocked, "Gameplay.AbilitiesBlocked", "Block the activation of any ability with input activation policy");

}
