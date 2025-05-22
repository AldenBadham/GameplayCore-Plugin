#pragma once

#include "AbilitySetHandles.h"
#include "AbilitySystemComponent.h"
#include "CoreMinimal.h"

#include "AbilitySet.generated.h"

struct FAbilitySet_GameplayAbility;
struct FAbilitySet_GameplayEffect;
struct FAbilitySet_AttributeSet;

/**
 *	UAbilitySet
 *
 *	Non-mutable data asset used to grant gameplay abilities, gameplay effects and attributes.
 */
UCLASS(BlueprintType, Const)
class ABILITYSYSTEMCORE_API UAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Grants the ability set to the specified ability system component.
	 * The returned handles can be used later to take away anything that was granted.
	 */
	void GiveToAbilitySystem(UAbilitySystemComponent* AbilitySystemComp, FAbilitySetHandles* Handles, UObject* SourceObject = nullptr) const;

protected:
	/** Gameplay abilities to grant when this ability set is given. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = "Ability"))
	TArray<FAbilitySet_GameplayAbility> GameplayAbilities;

	/** Gameplay effects to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = "Gameplay Effect"))
	TArray<FAbilitySet_GameplayEffect> GameplayEffects;

	/** Attribute sets to grant when this ability set is granted. */
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = "Attribute Set"))
	TArray<TSubclassOf<UAttributeSet>> AttributeSets;
};