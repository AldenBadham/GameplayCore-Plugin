// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/AbilitySet.h"

#include "Abilities/GameplayAbilityBase.h"
#include "Data/AbilitySet_GameplayAbility.h"
#include "Data/AbilitySet_GameplayEffect.h"
#include "Data/AbilitySetHandles.h"
#include "Log/AbilitySystemLog.h"

UAbilitySet::UAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
}

void UAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystemComp, FAbilitySetHandles* Handles, UObject* SourceObject) const
{
	check(AbilitySystemComp);
	if (!AbilitySystemComp->IsOwnerActorAuthoritative())
		UE_LOG(LogAbilitySystem, Error, TEXT("Must be authoritive to give or take ability sets"));

	// Grant the attribut sets
	for (int Index = 0; Index < AttributeSets.Num(); ++Index)
	{
		const TSubclassOf<UAttributeSet>& AttributeSetClass = AttributeSets[Index];

		UE_CLOG(!IsValid(AttributeSetClass), LogAbilitySystem, Error, TEXT("Attributes[%d] on ability set [%s] is not valid"), Index, *GetNameSafe(this));

		UAttributeSet* AttributeSet = NewObject<UAttributeSet>(AbilitySystemComp, AttributeSetClass);
		AbilitySystemComp->AddAttributeSetSubobject(AttributeSet);

		if (Handles)
		{
			Handles->AddAttributeSet(AttributeSet);
		}
	}

	// Grant the gameplay abilities
	for (int Index = 0; Index < GameplayAbilities.Num(); ++Index)
	{
		const FAbilitySet_GameplayAbility& GameplayAbilitySet = GameplayAbilities[Index];

		UE_CLOG(!IsValid(GameplayAbilitySet.Ability), LogAbilitySystem, Error, TEXT("Attributes[%d] on ability set [%s] is not valid"), Index, *GetNameSafe(this));

		UGameplayAbilityBase* AbilityCDO = GameplayAbilitySet.Ability->GetDefaultObject<UGameplayAbilityBase>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, GameplayAbilitySet.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(GameplayAbilitySet.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = AbilitySystemComp->GiveAbility(AbilitySpec);

		if (Handles)
		{
			Handles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 Index = 0; Index < GameplayEffects.Num(); ++Index)
	{
		const FAbilitySet_GameplayEffect& GameplayEffectSet = GameplayEffects[Index];

		UE_CLOG(!IsValid(GameplayEffectSet.GameplayEffect), LogAbilitySystem, Error, TEXT("Attributes[%d] on ability set [%s] is not valid"), Index, *GetNameSafe(this));

		const UGameplayEffect* GameplayEffect = GameplayEffectSet.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = AbilitySystemComp->ApplyGameplayEffectToSelf(GameplayEffect, GameplayEffectSet.EffectLevel, AbilitySystemComp->MakeEffectContext());

		if (Handles)
		{
			Handles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}
