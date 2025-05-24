// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "ActorFeatureData.h"
#include "ActorFeatureRegisteredDelegate.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "GameInitStateSubsystem.generated.h"

struct FActorFeatureState;

/**
 * This subsystem is highly based on ModularGameplay plugin's GameFrameworkComponentManager from Epic. To better handle initialization.
 *
 * The init state system can be used by components to coordinate their initialization using game-specific states specified as gameplay tags
 * IGameInitStateInterface provides a simple implementation that can be inherited by components
 */
UCLASS()
class GAMEPLAYCORE_API UGameInitStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Utility to get this manager from an actor, will return null if actor is null or not in a world */
	static UGameInitStateSubsystem* GetForActor(const AActor* Actor, bool bOnlyGameWorlds = true);

	/** Adds a new global actor feature state, either before or after an existing one. This will generally be called from game global or game feature initialization */
	void RegisterFeatureImplementer(AActor* Actor, FName FeatureName, UObject* Implementer, const TArray<FGameplayTag>& InitStateTags = {});

	/** Removes an actor and all of it's state information */
	void RemoveActorFeatureData(AActor* Actor);

	/** Removes an implementing object and any feature states it implements */
	void RemoveFeatureImplementer(AActor* Actor, UObject* Implementer);

	void SetFeatureInitStatTags(AActor* Actor, FName FeatureName, TArray<FGameplayTag> InitStateTags);

	/** Returns true if FeatureState comes after the second state (or is equal) */
	static bool IsInitStateCurrentOrLate(const FActorFeatureState& FeatureState, FGameplayTag RelativeState);

	/** Returns true if feature has reached query state or later */
	bool HasFeatureReachedInitState(AActor* Actor, FName FeatureName, FGameplayTag StateTag);

	/** Returns the earliest state found for the given feature */
	FGameplayTag GetInitStateForFeature(AActor* Actor, FName FeatureName) const;
	/** Returns the earliest state found for the given feature */
	TArray<FGameplayTag> GetAllInitStatesForFeature(AActor* Actor, FName FeatureName) const;

	/**
	 * Registers native delegate for feature state change notifications on a specific actor and may call it immediately
	 *
	 * @param Actor				The actor to listen for state changes to, if you don't have a specific actor call the Class version instead
	 * @param FeatureName		If not empty, only listen to state changes for the specified feature
	 * @param RequiredState		If specified, only activate if the init state of the feature is equal to or later than this
	 * @param Delegate			Native delegate to call
	 * @param bCallImmediately	If true and the actor feature is already in the specified state, call delegate immediately after registering
	 * @return DelegateHandle used for later removal
	 */
	FDelegateHandle RegisterAndCallForActorInitState(AActor* Actor, FName FeatureName, FGameplayTag RequiredState, FActorInitStateChangedDelegate Delegate, bool bCallImmediately = true);

	/** Removes a registered delegate bound to a specific actor */
	bool UnregisterActorInitStateDelegate(AActor* Actor, FDelegateHandle& Handle);

	/**
	 * Registers blueprint delegate for feature state change notifications on a specific actor and may call it immediately
	 *
	 * @param Actor				The actor to listen for state changes to, if you don't have a specific actor call the Class version instead
	 * @param FeatureName		If not empty, only listen to state changes for the specified feature
	 * @param RequiredState		If specified, only activate if the init state of the feature is equal to or later than this
	 * @param Delegate			Native delegate to call
	 * @param bCallImmediately	If true and the actor feature is already in the specified state, call delegate immediately after registering
	 * @return true if delegate was registered
	 */
	UFUNCTION(BlueprintCallable, Category = "InitState")
	bool RegisterAndCallForActorInitState(AActor* Actor, FName FeatureName, FGameplayTag RequiredState, FActorInitStateChangedBPDelegate Delegate, bool bCallImmediately = true);

	/** Removes a registered delegate bound to a specific actor */
	UFUNCTION(BlueprintCallable, Category = "InitState")
	bool UnregisterActorInitStateDelegate(AActor* Actor, FActorInitStateChangedBPDelegate DelegateToRemove);

private:
	/** Add to queue for delegate processing */
	void ProcessFeatureStateChange(AActor* Actor, const FActorFeatureState* StateChange);

	/** Call all delegates for a specific actor feature change */
	void CallFeatureStateDelegates(AActor* Actor, const FActorFeatureState& StateChange);

	/** Call the specified delegate for all matching features on the actor, this should be passed a copy of the original delegate */
	void CallDelegateForMatchingFeatures(AActor* Actor, const FActorFeatureRegisteredDelegate& RegisteredDelegate);

	/** Find an appropriate state struct if it exists */
	static const FActorFeatureState* FindFeatureStateStruct(const FActorFeatureData* FeatureData, FName FeatureName, FGameplayTag RequiredState = FGameplayTag());

	/** Gets or creates the actor struct */
	FActorFeatureData& FindOrAddActorData(AActor* Actor);

	/** Try to remove delegate from list, returns true if it removed anything and clear handle if required */
	static bool RemoveActorFeatureDelegateFromList(FActorFeatureDelegateList& DelegateList, FDelegateHandle& SearchHandle);
	static bool RemoveActorFeatureDelegateFromList(FActorFeatureDelegateList& DelegateList, const FActorInitStateChangedBPDelegate& SearchDelegate);

	/** Position in state change queue, INDEX_NONE means not actively handling */
	int32 CurrentStateChange = INDEX_NONE;

	/** A queue of state changes to call delegates for, we don't want recursive callbacks */
	TArray<TPair<AActor*, FActorFeatureState>> StateChangeQueue;

	/** Actors that were registered as tracking feature state */
	TMap<FObjectKey, FActorFeatureData> ActorFeatureMap;
};
