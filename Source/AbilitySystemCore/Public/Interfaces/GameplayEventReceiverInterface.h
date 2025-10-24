// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "UObject/Interface.h"
#include "GameplayEventReceiverInterface.generated.h"


UINTERFACE(BlueprintType)
class UGameplayEventReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ABILITYSYSTEMCORE_API IGameplayEventReceiverInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="GameplayEvent")
	void ReceiveGameplayEvent(FGameplayTag EventTag, const FGameplayEventData& Payload);
};
