// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ActorFeatureRegisteredDelegate.h"
#include "ActorFeatureState.h"

#include "ActorFeatureData.generated.h"

/** Always stored as shared refs to avoid being destroyed during execution */
using FActorFeatureDelegateList = TArray<TSharedRef<FActorFeatureRegisteredDelegate>>;

USTRUCT(BlueprintType)
/** Information for each registered actor */
struct GAMEPLAYCORE_API FActorFeatureData
{
	GENERATED_BODY()

	/** Actor class for cross-referencing with the class callbacks */
	TWeakObjectPtr<UClass> ActorClass;

	/** All active features */
	TArray<FActorFeatureState> RegisteredStates;

	/** All delegates bound to this actor */
	FActorFeatureDelegateList RegisteredDelegates;

	/** Properly clear delegate list */
	~FActorFeatureData();
};