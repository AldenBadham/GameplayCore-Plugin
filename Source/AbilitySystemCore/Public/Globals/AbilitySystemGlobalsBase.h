// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"

#include "AbilitySystemGlobalsBase.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYSYSTEMCORE_API UAbilitySystemGlobalsBase : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	
	/** Gets the single instance of the globals object, will create it as necessary. */
	static UAbilitySystemGlobalsBase& Get()
	{
		return *Cast<UAbilitySystemGlobalsBase>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
	}

	/** Initialize global tags by reading from config using the names and creating tags for use at runtime */
	virtual void InitGlobalTags() override;

	template <class T> static T* GetAbilitySystemComponentFromActor(const AActor* Actor, bool bLookForComponent = true)
	{
		return Cast<T>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, bLookForComponent));
	}
};
