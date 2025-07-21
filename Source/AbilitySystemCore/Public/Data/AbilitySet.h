// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "AbilitySetHandles.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "AbilitySet.generated.h"

struct FAbilitySet_GameplayAbility;
struct FAbilitySet_GameplayEffect;
struct FAbilitySet_AttributeSet;

/**
 *	UAbilitySet
 *
 *	Non-mutable data asset used to grant gameplay abilities, gameplay effects, and attributes.
 */
UCLASS(BlueprintType, Const)
class ABILITYSYSTEMCORE_API UAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Give the abilities, gameplay effects, and attributes to the specified ability system component.
	 * The returned handles can be used later to take away anything that was granted.
	 * @param AbilitySystemComp The ability system component to grant to.
	 * @param Handles The handles to store the granted abilities, effects, and attributes.
	 * @param SourceObject Optional source object for the abilities. */
	void GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystemComp, FAbilitySetHandles* Handles, UObject* SourceObject = nullptr) const;

protected:
	/** Gameplay abilities to grant when this ability set is given. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = "Ability"))
	TArray<FAbilitySet_GameplayAbility> GameplayAbilitySets;

	/** Gameplay effects to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = "Gameplay Effect"))
	TArray<FAbilitySet_GameplayEffect> GameplayEffectSets;

	/** Attribute sets to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = "Attribute Set"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;

	// GameplayTags to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Tags", meta = (GameplayTagFilter = "State"))
	FGameplayTagContainer GameplayTags;
};
