// // Licensed under the MIT License. See the LICENSE file in the project root for full license information.


#include "Library/AbilitySystemCoreLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Interfaces/GameplayEventReceiverInterface.h"

void UAbilitySystemCoreLibrary::SendGameplayEventToActor(AActor* Actor, FGameplayTag EventTag, FGameplayEventData Payload)
{
	if (::IsValid(Actor))
	{
		if (const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor); IsValidChecked(AbilitySystemComponent))
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EventTag, Payload);
		}
		else if (Actor->Implements<UGameplayEventReceiverInterface>())
		{
			IGameplayEventReceiverInterface::Execute_ReceiveGameplayEvent(Actor, EventTag, Payload);
		}
	}
}
