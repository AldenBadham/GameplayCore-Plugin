// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorInitStateChangedParams.h"
#include "GameplayTagContainer.h"

#include "ActorFeatureRegisteredDelegate.generated.h"

/** Native delegate called when an actor feature changes init state */
DECLARE_DELEGATE_OneParam(FActorInitStateChangedDelegate, const FActorInitStateChangedParams&);
/** Blueprint delegate called when an actor feature changes init state */
DECLARE_DYNAMIC_DELEGATE_OneParam(FActorInitStateChangedBPDelegate, const FActorInitStateChangedParams&, Params);

/** Holds the list of feature delegates */
USTRUCT(BlueprintType)
struct GAMEPLAYCORE_API FActorFeatureRegisteredDelegate
{
	GENERATED_BODY()

	FActorFeatureRegisteredDelegate()
		: bRemoved(false)
	{
	};
	/** Construct from a native or BP Delegate */
	FActorFeatureRegisteredDelegate(FActorInitStateChangedDelegate&& InDelegate, FName InFeatureName = NAME_None, FGameplayTag InInitState = FGameplayTag());
	FActorFeatureRegisteredDelegate(FActorInitStateChangedBPDelegate&& InDelegate, FName InFeatureName = NAME_None, FGameplayTag InInitState = FGameplayTag());

	/** Call the appropriate native/bp delegate, this could invalidate this struct */
	void Execute(AActor* OwningActor, FName FeatureName, UObject* Implementer, FGameplayTag FeatureState) const;

	/** Delegate that is called on notification */
	FActorInitStateChangedDelegate Delegate;

	/** BP delegate that is called on notification */
	FActorInitStateChangedBPDelegate BPDelegate;

	/** A handle assigned to this delegate so it acts like a multicast delegate for removal */
	FDelegateHandle DelegateHandle;

	/** If this is not null, will only activate for specific feature names */
	FName RequiredFeatureName;

	/** If this is not null, will only activate for states >= to this */
	FGameplayTag RequiredInitState;

	/** Indicates this delegate has been removed and will soon be destroyed, do not execute */
	bool bRemoved;
};
