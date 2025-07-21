// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Instances/Components/ItemComponent_Consumable.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Definitions/Fragments/ItemFragment_Consumable.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Instances/ItemInstance.h"
#include "Log/InventorySystemLog.h"
#include "Net/UnrealNetwork.h"

void UItemComponent_Consumable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemComponent_Consumable, RemainingUses);
}

bool UItemComponent_Consumable::CanConsume(const int32 UseCount) const
{
	return RemainingUses >= UseCount;
}

void UItemComponent_Consumable::Consume(const int32 UseCount)
{
	if (!CanConsume(UseCount) && !IsValid(GameplayEffect))
	{
		return;
	}

	const AActor* OwnerActor = OwningInstance->GetOwningActor();
	if (!OwnerActor)
	{
		return;
	}
	
	if (UAbilitySystemComponent* const TargetAbilityComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor))
	{
		FGameplayEffectContextHandle EffectContext = TargetAbilityComponent->MakeEffectContext();
		EffectContext.AddSourceObject(OwningInstance.Get());

		const int32 EffectCount = ShouldStackEffects ? UseCount : 1;
		
		// Even if one time has succeeded, reduce the count of uses left.
		bool Succeeded = false;
		for (int32 i = 0; i < EffectCount; ++i)
		{
			const UGameplayEffect* const GameplayEffectCDO = GameplayEffect->GetDefaultObject<UGameplayEffect>();
			const FActiveGameplayEffectHandle GameplayEffectHandle = TargetAbilityComponent->ApplyGameplayEffectToSelf(GameplayEffectCDO, 0, EffectContext);

			Succeeded |= GameplayEffectHandle.WasSuccessfullyApplied();
		}
			
		if (Succeeded)
		{
			RemainingUses -= UseCount;
		}
	}
}

void UItemComponent_Consumable::SetRemainingUses(const int32 Count)
{
	UE_CLOG(Count > MaxUseCount, LogInventorySystem, Warning, TEXT("Tried to set remaining uses for consumable item [%s] to %d but clamps to maximum use count %d."), *GetNameSafe(OwningInstance->GetDefinitionClass()), Count, MaxUseCount);
	RemainingUses = FMath::Clamp(Count, 0, MaxUseCount);
}

void UItemComponent_Consumable::RestoreUses()
{
	RemainingUses = MaxUseCount;
}