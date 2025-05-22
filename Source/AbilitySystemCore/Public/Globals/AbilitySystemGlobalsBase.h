// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "AbilitySystemGlobals.h"
#include "CoreMinimal.h"

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
	static UAbilitySystemGlobalsBase& Get() { return *Cast<UAbilitySystemGlobalsBase>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()); }

	/** Initialize global tags by reading from config using the names and creating tags for use at runtime */
	virtual void InitGlobalTags() override;

	template <class T> static T* GetAbilitySystemComponentFromActor(const AActor* Actor, const bool bLookForComponent = true)
	{
		return Cast<T>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, bLookForComponent));
	}
};