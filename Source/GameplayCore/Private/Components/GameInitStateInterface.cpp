// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/GameInitStateInterface.h"

#include "Components/ActorFeatureRegisteredDelegate.h"
#include "Components/GameInitStateSubsystem.h"
#include "GameplayTagContainer.h"

// Add default functionality here for any IGameInitStateInterface functions that are not pure virtual.
AActor* IGameInitStateInterface::GetOwningActor() const
{
	// Removing const because most AActor functions can't handle const
	AActor* FoundActor = const_cast<AActor*>(Cast<AActor>(this));

	if (!FoundActor)
	{
		if (const UActorComponent* FoundComponent = Cast<UActorComponent>(this))
		{
			FoundActor = FoundComponent->GetOwner();
		}
	}

	if (ensure(FoundActor))
	{
		// Subclasses must implement this if they are not actors or components
		return FoundActor;
	}

	return nullptr;
}

UGameInitStateSubsystem* IGameInitStateInterface::GetGameInitStateSubsystem() const
{
	return UGameInitStateSubsystem::GetForActor(GetOwningActor());
}

FGameplayTag IGameInitStateInterface::GetCurrentInitStateFor(FName& FeatureName) const
{
	AActor* OwningActor = GetOwningActor();

	if (const UGameInitStateSubsystem* Manager = UGameInitStateSubsystem::GetForActor(OwningActor))
	{
		return Manager->GetInitStateForFeature(OwningActor, FeatureName);
	}

	return FGameplayTag();
}

bool IGameInitStateInterface::HasReachedInitState(const FName FeatureName, const FGameplayTag DesiredState) const
{
	AActor* OwningActor = GetOwningActor();

	if (UGameInitStateSubsystem* Manager = UGameInitStateSubsystem::GetForActor(OwningActor))
	{
		return Manager->HasFeatureReachedInitState(OwningActor, FeatureName, DesiredState);
	}

	return false;
}

TArray<FGameplayTag> IGameInitStateInterface::GetAllInitStatesFor(FName& FeatureName) const
{
	AActor* OwningActor = GetOwningActor();

	if (const UGameInitStateSubsystem* Manager = UGameInitStateSubsystem::GetForActor(OwningActor))
	{
		return Manager->GetAllInitStatesForFeature(OwningActor, FeatureName);
	}

	return {};
}

void IGameInitStateInterface::RegisterInitStateFeature(FName& FeatureName)
{
	UObject* ThisObject = Cast<UObject>(this);
	AActor* OwningActor = GetOwningActor();

	UGameInitStateSubsystem* Manager = UGameInitStateSubsystem::GetForActor(OwningActor);

	if (OwningActor && Manager)
	{
		// Manager will be null if this isn't in a game world
		Manager->RegisterFeatureImplementer(OwningActor, FeatureName, ThisObject);
	}
}

void IGameInitStateInterface::UnregisterInitStateFeature(FName& FeatureName)
{
	UObject* ThisObject = Cast<UObject>(this);
	AActor* OwningActor = GetOwningActor();

	if (UGameInitStateSubsystem* Manager = UGameInitStateSubsystem::GetForActor(OwningActor))
	{
		if (ThisObject == OwningActor)
		{
			// This will clear all the feature states and delegates
			Manager->RemoveActorFeatureData(OwningActor);
		}
		else
		{
			Manager->RemoveFeatureImplementer(OwningActor, ThisObject);
		}

		if (ActorInitStateChangedHandle.IsValid())
		{
			Manager->UnregisterActorInitStateDelegate(OwningActor, ActorInitStateChangedHandle);
		}
	}
}

void IGameInitStateInterface::BindOnActorInitStateChanged(const FName FeatureName, const FGameplayTag RequiredState, const bool bCallIfReached)
{
	UObject* ThisObject = Cast<UObject>(this);
	AActor* OwningActor = GetOwningActor();

	UGameInitStateSubsystem* Manager = UGameInitStateSubsystem::GetForActor(OwningActor);

	if (ensure(OwningActor && Manager))
	{
		// Bind as a weak lambda because this is not a UObject but is guaranteed to be valid as long as ThisObject is
		FActorInitStateChangedDelegate Delegate = FActorInitStateChangedDelegate::CreateWeakLambda(ThisObject, [this](const FActorInitStateChangedParams& Params) { this->OnActorInitStateChanged(Params); });

		ActorInitStateChangedHandle = Manager->RegisterAndCallForActorInitState(OwningActor, FeatureName, RequiredState, MoveTemp(Delegate), bCallIfReached);
	}
}

bool IGameInitStateInterface::RegisterAndCallForActorInitState(const FName FeatureName, const FGameplayTag RequiredState, const FActorInitStateChangedBPDelegate Delegate, const bool bCallImmediately)
{
	AActor* OwningActor = GetOwningActor();

	UGameInitStateSubsystem* Manager = UGameInitStateSubsystem::GetForActor(OwningActor);

	if (ensure(OwningActor && Manager))
	{
		return Manager->RegisterAndCallForActorInitState(OwningActor, FeatureName, RequiredState, Delegate, bCallImmediately);
	}

	return false;
}

bool IGameInitStateInterface::UnregisterInitStateDelegate(const FActorInitStateChangedBPDelegate Delegate)
{
	AActor* OwningActor = GetOwningActor();

	if (UGameInitStateSubsystem* Manager = UGameInitStateSubsystem::GetForActor(OwningActor); ensure(OwningActor && Manager))
	{
		return Manager->UnregisterActorInitStateDelegate(OwningActor, Delegate);
	}

	return false;
}

FString IGameInitStateInterface::GetDebugState(FName FeatureName) const
{
	return FString::Printf(TEXT("CurrentState: %s"), *GetCurrentInitStateFor(FeatureName).ToString());
}
