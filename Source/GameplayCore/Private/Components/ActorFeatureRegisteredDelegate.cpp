// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/ActorFeatureRegisteredDelegate.h"


FActorFeatureRegisteredDelegate::FActorFeatureRegisteredDelegate(FActorInitStateChangedDelegate&& InDelegate, FName InFeatureName, FGameplayTag InInitState) : Delegate(InDelegate)
	, DelegateHandle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle)
	, RequiredFeatureName(InFeatureName)
	, RequiredInitState(InInitState)
	, bRemoved(false)
{
	
}

FActorFeatureRegisteredDelegate::FActorFeatureRegisteredDelegate(FActorInitStateChangedBPDelegate&& InDelegate, FName InFeatureName, FGameplayTag InInitState) : BPDelegate(InDelegate)
	, DelegateHandle(FDelegateHandle::EGenerateNewHandleType::GenerateNewHandle)
	, RequiredFeatureName(InFeatureName)
	, RequiredInitState(InInitState)
	, bRemoved(false)
{
}

void FActorFeatureRegisteredDelegate::Execute(AActor* OwningActor, FName FeatureName, UObject* Implementer, FGameplayTag FeatureState)
{
	if (bRemoved)
	{
		return;
	}

	FActorInitStateChangedParams Params(OwningActor, FeatureName, Implementer, FeatureState);
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
