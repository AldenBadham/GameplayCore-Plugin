#include "Components/GameInitStateSubsystem.h"
#include "Components/ActorFeatureData.h"
#include "Components/ActorFeatureState.h"

void UGameInitStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

	Super::Initialize(Collection);

	CurrentStateChange = INDEX_NONE;
}

void UGameInitStateSubsystem::Deinitialize()
{
	Super::Deinitialize();

	#if WITH_EDITORONLY_DATA
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
	#endif
}

UGameInitStateSubsystem* UGameInitStateSubsystem::GetForActor(const AActor* Actor, const bool bOnlyGameWorlds)
{
	if (Actor)
	{
		if (const UWorld* ActorWorld = Actor->GetWorld())
		{
			if (bOnlyGameWorlds && (!ActorWorld->IsGameWorld() || ActorWorld->IsPreviewWorld()))
			{
				return nullptr;
			}

			return UGameInstance::GetSubsystem<UGameInitStateSubsystem>(ActorWorld->GetGameInstance());
		}
	}

	return nullptr;
}

void UGameInitStateSubsystem::RegisterFeatureImplementer(AActor* Actor, FName FeatureName, UObject* Implementer, const TArray<FGameplayTag>& InitStateTags)
{
	ensure(Actor == nullptr || FeatureName.IsNone());

	auto& [ActorClass, RegisteredStates, RegisteredDelegates] = FindOrAddActorData(Actor);

	FActorFeatureState* FoundState = RegisteredStates.FindByPredicate([FeatureName](const FActorFeatureState& State) { return State.FeatureName == FeatureName; });
	if (!FoundState)
	{
		// We've not found a matching Feature State, create a new one.
		FoundState = &RegisteredStates.Emplace_GetRef(FeatureName);
	}

	FoundState->Implementer = Implementer;
	FoundState->States = InitStateTags;
}

void UGameInitStateSubsystem::RemoveActorFeatureData(AActor* Actor)
{
	ActorFeatureMap.Remove(FObjectKey(Actor));
}

void UGameInitStateSubsystem::RemoveFeatureImplementer(AActor* Actor, UObject* Implementer)
{
	if (Actor == nullptr || Implementer == nullptr)
	{
		return;
	}
	TWeakObjectPtr WeakToRemove(Implementer);

	if (FActorFeatureData* FoundStruct = ActorFeatureMap.Find(FObjectKey(Actor)))
	{
		for (int32 i = FoundStruct->RegisteredStates.Num() - 1; i >= 0; i--)
		{
			// Clear if it matches or is stale
			const UObject* ResolvedObject = FoundStruct->RegisteredStates[i].Implementer.Get();

			if (ResolvedObject == Implementer || (!ResolvedObject && !FoundStruct->RegisteredStates[i].Implementer.IsExplicitlyNull()))
			{
				FoundStruct->RegisteredStates.RemoveAt(i);
			}
		}
	}
}

void UGameInitStateSubsystem::SetFeatureInitStatTags(AActor* Actor, FName FeatureName, TArray<FGameplayTag> InitStateTags)
{
	ensure(Actor == nullptr || FeatureName.IsNone());

	FActorFeatureData& FeatureData = FindOrAddActorData(Actor);

	FActorFeatureState* FoundState = FeatureData.RegisteredStates.FindByPredicate([FeatureName](const FActorFeatureState& State) { return State.FeatureName == FeatureName; });
	if (FoundState)
	{
		FoundState->States = InitStateTags;
		FoundState->CurrentState = InitStateTags[0];
	}

	ProcessFeatureStateChange(Actor, FoundState);
}

bool UGameInitStateSubsystem::IsInitStateCurrentOrLate(const FActorFeatureState& FeatureState, const FGameplayTag RelativeState)
{
	if (!FeatureState.IsValid())
	{
		return false;
	}

	if (FeatureState.CurrentState == RelativeState)
	{
		return true;
	}

	const int32 CurrentStateIndex = FeatureState.States.IndexOfByKey(FeatureState.CurrentState);
	const int32 RelateStateIndex = FeatureState.States.IndexOfByKey(RelativeState);

	return RelateStateIndex > CurrentStateIndex;
}

bool UGameInitStateSubsystem::HasFeatureReachedInitState(AActor* Actor, FName FeatureName, const FGameplayTag StateTag)
{
	FActorFeatureData* FeatureData = ActorFeatureMap.Find(Actor);
	const FActorFeatureState* FoundState = FeatureData->RegisteredStates.FindByPredicate([FeatureName](const FActorFeatureState& State) { return State.FeatureName == FeatureName; });
	return IsInitStateCurrentOrLate(*FoundState, StateTag);
}

FGameplayTag UGameInitStateSubsystem::GetInitStateForFeature(AActor* Actor, const FName FeatureName) const
{
	if (const FActorFeatureState* FoundState = FindFeatureStateStruct(ActorFeatureMap.Find(FObjectKey(Actor)), FeatureName, FGameplayTag()))
	{
		return FoundState->CurrentState;
	}

	return FGameplayTag();
}

TArray<FGameplayTag> UGameInitStateSubsystem::GetAllInitStatesForFeature(AActor* Actor, const FName FeatureName) const
{
	if (const FActorFeatureState* FoundState = FindFeatureStateStruct(ActorFeatureMap.Find(FObjectKey(Actor)), FeatureName, FGameplayTag()))
	{
		return FoundState->States;
	}
	return {};
}

FDelegateHandle UGameInitStateSubsystem::RegisterAndCallForActorInitState(AActor* Actor, FName FeatureName, FGameplayTag RequiredState, FActorInitStateChangedDelegate Delegate, const bool bCallImmediately)
{
	if (ensure(Actor && Delegate.IsBound()))
	{
		// We often register delegates before registering states
		FActorFeatureData& ActorStruct = FindOrAddActorData(Actor);

		const TSharedRef<FActorFeatureRegisteredDelegate> RegisteredDelegate = MakeShared<FActorFeatureRegisteredDelegate>(MoveTemp(Delegate), FeatureName, RequiredState);
		ActorStruct.RegisteredDelegates.Add(RegisteredDelegate);

		if (bCallImmediately)
		{
			// The shared ref keeps the delegate alive in case it gets unregistered
			CallDelegateForMatchingFeatures(Actor, *RegisteredDelegate);
		}

		return RegisteredDelegate->DelegateHandle;
	}

	return FDelegateHandle();
}

bool UGameInitStateSubsystem::UnregisterActorInitStateDelegate(AActor* Actor, FDelegateHandle& Handle)
{
	if (Actor && Handle.IsValid())
	{
		if (FActorFeatureData* ActorStruct = ActorFeatureMap.Find(FObjectKey(Actor)))
		{
			return RemoveActorFeatureDelegateFromList(ActorStruct->RegisteredDelegates, Handle);
		}
	}

	return false;
}

bool UGameInitStateSubsystem::RegisterAndCallForActorInitState(AActor* Actor, FName FeatureName, FGameplayTag RequiredState, FActorInitStateChangedBPDelegate Delegate, const bool bCallImmediately /*= true*/)
{
	if (ensure(Actor && Delegate.IsBound()))
	{
		// We often register delegates before registering states
		FActorFeatureData& ActorStruct = FindOrAddActorData(Actor);

		const TSharedRef<FActorFeatureRegisteredDelegate> RegisteredDelegate = MakeShared<FActorFeatureRegisteredDelegate>(MoveTemp(Delegate), FeatureName, RequiredState);
		ActorStruct.RegisteredDelegates.Add(RegisteredDelegate);

		if (bCallImmediately)
		{
			// The shared ref keeps the delegate alive in case it gets unregistered
			CallDelegateForMatchingFeatures(Actor, *RegisteredDelegate);
		}

		return true;
	}

	return false;
}

bool UGameInitStateSubsystem::UnregisterActorInitStateDelegate(AActor* Actor, const FActorInitStateChangedBPDelegate DelegateToRemove)
{
	if (Actor && DelegateToRemove.IsBound())
	{
		if (FActorFeatureData* ActorStruct = ActorFeatureMap.Find(FObjectKey(Actor)))
		{
			return RemoveActorFeatureDelegateFromList(ActorStruct->RegisteredDelegates, DelegateToRemove);
		}
	}

	return false;
}

void UGameInitStateSubsystem::ProcessFeatureStateChange(AActor* Actor, const FActorFeatureState* StateChange)
{
	StateChangeQueue.Emplace(Actor, *StateChange);

	if (CurrentStateChange == INDEX_NONE)
	{
		// Start processing in order
		CurrentStateChange = 0;

		while (CurrentStateChange < StateChangeQueue.Num())
		{
			CallFeatureStateDelegates(StateChangeQueue[CurrentStateChange].Key, StateChangeQueue[CurrentStateChange].Value);
			CurrentStateChange++;
		}

		// Done processing, clear it
		StateChangeQueue.Empty();
		CurrentStateChange = INDEX_NONE;
	}
}

void UGameInitStateSubsystem::CallFeatureStateDelegates(AActor* Actor, const FActorFeatureState& StateChange)
{
	FActorFeatureData* ActorStruct = ActorFeatureMap.Find(FObjectKey(Actor));
	FActorFeatureDelegateList QueuedDelegates;

	// Should only be called inside ProcessFeatureStateChange
	ensure(CurrentStateChange != INDEX_NONE);

	if (ActorStruct)
	{
		for (TSharedRef<FActorFeatureRegisteredDelegate>& DelegateRef : ActorStruct->RegisteredDelegates)
		{
			if (FActorFeatureRegisteredDelegate& RegisteredDelegate = *DelegateRef; RegisteredDelegate.RequiredFeatureName == StateChange.FeatureName && IsInitStateCurrentOrLate(
				StateChange,
				RegisteredDelegate.RequiredInitState))
			{
				// Queue delegates now in case the registered list changes during execution
				// If new delegates are registered, they are handled at registration time if bCallImmediately is used
				QueuedDelegates.Add(DelegateRef);
			}
		}
	}

	// Now execute the queued delegates, if they are removed Execute will skip them
	for (const TSharedRef<FActorFeatureRegisteredDelegate>& QueuedDelegate : QueuedDelegates)
	{
		QueuedDelegate->Execute(Actor, StateChange.FeatureName, StateChange.Implementer.Get(), StateChange.CurrentState);
	}
}

void UGameInitStateSubsystem::CallDelegateForMatchingFeatures(AActor* Actor, const FActorFeatureRegisteredDelegate& RegisteredDelegate)
{
	const FActorFeatureData* ActorStruct = ActorFeatureMap.Find(FObjectKey(Actor));

	if (ActorStruct == nullptr)
	{
		return;
	}

	// The feature name is mandatory on our version
	if (RegisteredDelegate.RequiredFeatureName.IsNone())
	{
		return;
	}

	if (const FActorFeatureState* FoundState = FindFeatureStateStruct(ActorStruct, RegisteredDelegate.RequiredFeatureName, RegisteredDelegate.RequiredInitState))
	{
		RegisteredDelegate.Execute(Actor, FoundState->FeatureName, FoundState->Implementer.Get(), FoundState->CurrentState);
	}
}

const FActorFeatureState* UGameInitStateSubsystem::FindFeatureStateStruct(const FActorFeatureData* FeatureData, const FName FeatureName, const FGameplayTag RequiredState)
{
	if (FeatureData)
	{
		for (const FActorFeatureState& State : FeatureData->RegisteredStates)
		{
			// We don't want to return invalid states even if it's the valid FeatureName
			if (State.IsValid() && State.FeatureName == FeatureName)
			{
				if (IsInitStateCurrentOrLate(State, RequiredState))
				{
					return &State;
				}
			}
		}
	}

	return nullptr;
}

bool UGameInitStateSubsystem::RemoveActorFeatureDelegateFromList(FActorFeatureDelegateList& DelegateList, FDelegateHandle& SearchHandle)
{
	for (int32 i = DelegateList.Num() - 1; i >= 0; i--)
	{
		if (DelegateList[i]->DelegateHandle == SearchHandle)
		{
			DelegateList[i]->bRemoved = true;
			DelegateList.RemoveAt(i);
			SearchHandle.Reset();
			return true;
		}
	}

	return false;
}

bool UGameInitStateSubsystem::RemoveActorFeatureDelegateFromList(FActorFeatureDelegateList& DelegateList, const FActorInitStateChangedBPDelegate& SearchDelegate)
{
	for (int32 i = DelegateList.Num() - 1; i >= 0; i--)
	{
		if (DelegateList[i]->BPDelegate == SearchDelegate)
		{
			DelegateList[i]->bRemoved = true;
			DelegateList.RemoveAt(i);
			return true;
		}
	}

	return false;
}

FActorFeatureData& UGameInitStateSubsystem::FindOrAddActorData(AActor* Actor)
{
	check(Actor)

	FActorFeatureData& FeatureData = ActorFeatureMap.FindOrAdd(Actor);
	if (!FeatureData.ActorClass.IsValid())
	{
		FeatureData.ActorClass = Actor->GetClass();
	}
	return FeatureData;
}