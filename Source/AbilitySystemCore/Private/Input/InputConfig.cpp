// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Input/InputConfig.h"
#include "Input/InputTagSet.h"

#include "Log/AbilitySystemLog.h"

UInputConfig::UInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get()) {}

const UInputAction* UInputConfig::FindInputActionForTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const auto& [InputAction, InputTag] : InputTagSets)
	{
		if (InputAction && InputTag == Tag)
		{
			return InputAction;
		}
	}

	UE_LOG(LogAbilitySystem, Error, TEXT("Can't find InputAction matching InputTag [%s] on InputConfig [%s]."), *Tag.ToString(), *GetNameSafe(this));
	return nullptr;
}