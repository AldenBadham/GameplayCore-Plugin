#pragma once

#include "GameplayTagContainer.h"

#include "ActorFeatureState.generated.h"

/** State for a specific object implementing an actor feature, should this be in a map instead of an array? */
USTRUCT(BlueprintType)
struct GAMEPLAYCORE_API FActorFeatureState
{
	GENERATED_BODY()

	FActorFeatureState() {};

	FActorFeatureState(const FName InFeatureName)
		: FeatureName(InFeatureName) {}

	/** The feature this is tracking */
	FName FeatureName;

	/** The state when it was last registered */
	FGameplayTag CurrentState;

	TArray<FGameplayTag> States = {};

	/** The object implementing this feature, this can be null */
	TWeakObjectPtr<UObject> Implementer;

	bool IsValid() const { return !(FeatureName.IsNone() || States.IsEmpty() || !States.Contains(CurrentState)); }
};