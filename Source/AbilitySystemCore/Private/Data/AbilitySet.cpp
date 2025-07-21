// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/AbilitySet.h"

#include "AbilitySystemComponent.h"
#include "Data/AbilitySetHandles.h"
#include "Data/AbilitySet_GameplayAbility.h"
#include "Data/AbilitySet_GameplayEffect.h"
#include "GameplayEffects/UGameplayEffect_Infinite.h"
#include "GameplayTags/AbilitySystemTags.h"
#include "Log/AbilitySystemLog.h"

UAbilitySet::UAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
}

void UAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystemComp, FAbilitySetHandles* Handles, UObject* SourceObject) const
{
	check(AbilitySystemComp);
	if (!AbilitySystemComp->IsOwnerActorAuthoritative())
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("Must be authoritative to give or take ability sets"));
		return;
	}


	// Grant the gameplay abilities
	for (int Index = 0; Index < GameplayAbilitySets.Num(); ++Index)
	{
		const auto& [Ability, Level, InputTag] = GameplayAbilitySets[Index];
		if (!IsValid(Ability))
		{
			UE_LOG(LogAbilitySystem, Error, TEXT("Ability [%d] on ability set [%s] is not valid"), Index, *GetNameSafe(this));
			continue;
		}

		UGameplayAbilityBase* const AbilityCDO = Ability->GetDefaultObject<UGameplayAbilityBase>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, Level);
		AbilitySpec.SourceObject = SourceObject;

		if (InputTag.IsValid())
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(InputTag);
		}

		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComp->GiveAbility(AbilitySpec);

		if (Handles)
		{
			Handles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int Index = 0; Index < GameplayEffectSets.Num(); ++Index)
	{
		const auto& [GameplayEffect, Level] = GameplayEffectSets[Index];

		UE_CLOG(!IsValid(GameplayEffect), LogAbilitySystem, Error, TEXT("GameplayEffect [%d] on ability set [%s] is not valid"), Index, *GetNameSafe(this));

		const UGameplayEffect* const GameplayEffectCDO = GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = AbilitySystemComp->ApplyGameplayEffectToSelf(GameplayEffectCDO, Level, AbilitySystemComp->MakeEffectContext());

		if (Handles)
		{
			Handles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets
	for (int Index = 0; Index < AttributeSets.Num(); ++Index)
	{
		const TSubclassOf<UAttributeSet>& AttributeSetClass = AttributeSets[Index];

		UE_CLOG(!IsValid(AttributeSetClass), LogAbilitySystem, Error, TEXT("Attributes [%d] on ability set [%s] is not valid"), Index, *GetNameSafe(this));

		UAttributeSet* AttributeSet = NewObject<UAttributeSet>(AbilitySystemComp, AttributeSetClass);
		AbilitySystemComp->AddAttributeSetSubobject(AttributeSet);

		if (Handles)
		{
			Handles->AddAttributeSet(AttributeSet);
		}
	}

	// Give gameplay tags by granting an infinite gameplay effect
	const FGameplayEffectContextHandle Context = AbilitySystemComp->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComp->MakeOutgoingSpec(UGameplayEffect_Infinite::StaticClass(), 1.0, Context);

	if (FGameplayEffectSpec* const Spec = SpecHandle.Data.Get())
	{
		Spec->DynamicGrantedTags = GameplayTags;
		Spec->AddDynamicAssetTag(AbilitySystemTags::TAG_DATA_EFFECT_PERSISTENT);

		AbilitySystemComp->ApplyGameplayEffectSpecToSelf(*Spec, AbilitySystemComp->GetPredictionKeyForNewAction());
	}
}
