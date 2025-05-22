// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/ActorFeatureRegisteredDelegate.h"

FActorFeatureRegisteredDelegate::FActorFeatureRegisteredDelegate(FActorInitStateChangedDelegate&& InDelegate, const FName InFeatureName, const FGameplayTag InInitState)
	: Delegate(InDelegate), DelegateHandle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle), RequiredFeatureName(InFeatureName), RequiredInitState(InInitState), bRemoved(false) {}

FActorFeatureRegisteredDelegate::FActorFeatureRegisteredDelegate(FActorInitStateChangedBPDelegate&& InDelegate, const FName InFeatureName, const FGameplayTag InInitState)
	: BPDelegate(InDelegate), DelegateHandle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle), RequiredFeatureName(InFeatureName), RequiredInitState(InInitState), bRemoved(false) {}

void FActorFeatureRegisteredDelegate::Execute(AActor* OwningActor, const FName FeatureName, UObject* Implementer, const FGameplayTag FeatureState) const
{
	if (bRemoved)
	{
		return;
	}

	const FActorInitStateChangedParams Params(OwningActor, FeatureName, Implementer, FeatureState);
	if (Delegate.IsBound())
	{
		ensure(!BPDelegate.IsBound());

		Delegate.Execute(Params);
	}
	else if (BPDelegate.IsBound())
	{
		BPDelegate.Execute(Params);
	}
}