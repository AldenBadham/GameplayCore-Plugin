// // Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"

#include "UGameplayEffect_Infinite.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ABILITYSYSTEMCORE_API UGameplayEffect_Infinite : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGameplayEffect_Infinite()
	{
		DurationPolicy = EGameplayEffectDurationType::Infinite;
	}
};
