// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayTags/GameplayCoreTags.h"

namespace GameplayCoreTags
{
	// Plugin bases init state tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_Spawned, "InitState.Spawned", "Actor/component has initially spawned and can be extended");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_DataAvailable, "InitState.DataAvailable", "All required data has been loaded/replicated and is ready for initialization");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_DataInitialized, "InitState.DataInitialized", "The available data has been initialized for this actor/component, but it is not ready for full gameplay");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_InitState_GameplayReady, "InitState.GameplayReady", "The actor/component is fully ready for active gameplay");
} // namespace GameplayCoreTags
