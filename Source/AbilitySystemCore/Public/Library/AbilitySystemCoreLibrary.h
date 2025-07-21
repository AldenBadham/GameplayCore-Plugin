// // Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemCoreLibrary.generated.h"

struct FGameplayEventData;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class ABILITYSYSTEMCORE_API UAbilitySystemCoreLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * This function can be used to send a gameplay event to the given actor with useful payload data.
	 * NOTE: Will send the event if the actor implements the IAbilitySystemInterface or the IGameplayEventReceiverInterface,
	 * else will not be sent.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability", Meta = (Tooltip = "This function can be used to send a gameplay event to the given actor with useful payload data."))
	static void SendGameplayEventToActor(AActor* Actor, UPARAM(meta=(GameplayTagFilter="GameplayEvent")) FGameplayTag EventTag, FGameplayEventData Payload);
	
};
