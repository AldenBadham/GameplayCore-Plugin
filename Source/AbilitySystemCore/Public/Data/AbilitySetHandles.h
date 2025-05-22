// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "GameplayAbilitySpecHandle.h"

#include "AbilitySetHandles.generated.h"

/**
 * FAbilitySetHandles
 *
 *	Data used to store handles to what has been granted by the ability set.
 */
USTRUCT(BlueprintType)
struct ABILITYSYSTEMCORE_API FAbilitySetHandles
{
	GENERATED_BODY()

	/**
	 * Adds a gameplay ability spec handle to the set.
	 * @param Handle The gameplay ability spec handle to add.
	 */
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	/**
	 * Adds a gameplay effect handle to the set.
	 * @param Handle The gameplay effect handle to add.
	 */
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	/**
	 * Adds an attribute set to the granted sets.
	 * @param Set The attribute set to add.
	 */
	void AddAttributeSet(UAttributeSet* Set);

	/**
	 * Removes all granted abilities, effects, and attributes from the given ability system component.
	 * @param AbilitySystemComp The ability system component to remove granted items from.
	 */
	void TakeFromAbilitySystem(UAbilitySystemComponent* AbilitySystemComp);

protected:
	/** Handles to the granted abilities. */
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	/** Handles to the granted gameplay effects. */
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	/** Pointers to the granted attribute sets. */
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};