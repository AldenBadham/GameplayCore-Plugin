// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "GameInitStateInterface.generated.h"

class FActorInitStateChangedBPDelegate;
class UGameInitStateSubsystem;
struct FActorInitStateChangedParams;
struct FActorFeatureRegisteredDelegate;

/** Interface that can be implemented by actors/components to make interacting with the init state system easier */
UINTERFACE(NotBlueprintable)
class GAMEPLAYCORE_API UGameInitStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class GAMEPLAYCORE_API IGameInitStateInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Returns the Actor this object is bound to, might be this object */
	virtual AActor* GetOwningActor() const;

	/** Gets the component manager corresponding to this object based on owning actor */
	UGameInitStateSubsystem* GetGameInitStateSubsystem() const;

	/** Returns the current feature state of this object, the default behavior is to query the manager */
	UFUNCTION(BlueprintCallable, Category = "InitState")
	virtual FGameplayTag GetCurrentInitStateFor(FName& FeatureName) const;

	/** Checks the component manager to see if we have already reached the desired state or a later one */
	UFUNCTION(BlueprintCallable, Category = "InitState")
	virtual bool HasReachedInitState(FName FeatureName, FGameplayTag DesiredState) const;

	/** Returns the current feature state of this object, the default behavior is to query the manager */
	UFUNCTION(BlueprintCallable, Category = "InitState")
	virtual TArray<FGameplayTag> GetAllInitStatesFor(FName& FeatureName) const;

	/** Override to try and progress the default initialization path, likely using ContinueInitStateChain */
	virtual void CheckDefaultInitialization() {}

	/** Signature for handling a game feature state, this is not registered by default */
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) {}

	/** Call to register with the component manager during spawn if this is a game world */
	virtual void RegisterInitStateFeature(FName& FeatureName);

	/** Unregisters state and delegate binding with component manager */
	virtual void UnregisterInitStateFeature(FName& FeatureName);

	/** Call to bind the OnActorInitStateChanged function to the appropriate delegate on the component manager */
	virtual void BindOnActorInitStateChanged(FName FeatureName, FGameplayTag RequiredState, bool bCallIfReached);

	/** Binds a BP delegate to get called on a state change for this feature */
	UFUNCTION(BlueprintCallable, Category = "InitState")
	virtual bool RegisterAndCallForActorInitState(FName FeatureName, FGameplayTag RequiredState, FActorInitStateChangedBPDelegate Delegate, bool bCallImmediately = true);

	/** Unbinds a BP delegate from changes to this feature */
	UFUNCTION(BlueprintCallable, Category = "InitState")
	virtual bool UnregisterInitStateDelegate(FActorInitStateChangedBPDelegate Delegate);

	/** Returns Current state and any additional debug information for the active state */
	virtual FString GetDebugState(FName FeatureName) const;

protected:
	/** Default handle created from calling BindOnActorInitStateChanged */
	FDelegateHandle ActorInitStateChangedHandle;
};