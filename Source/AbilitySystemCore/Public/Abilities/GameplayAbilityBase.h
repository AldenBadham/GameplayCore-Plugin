// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "AbilityActivationPolicy.h"
#include "CoreMinimal.h"

#include "GameplayAbilityBase.generated.h"

/**
 * UGameplayAbilityBase
 *
 * Abilities define custom game logic that can be activated by players or by external game logic.
 */
UCLASS(Abstract, Blueprintable)
class ABILITYSYSTEMCORE_API UGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

	friend class UAbilitySystemComponentBase;

public:
	explicit UGameplayAbilityBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Gets the activation policy for this ability.
	 * @return The activation policy.
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability")
	EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	// UGameplayAbility
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	[[nodiscard]] virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// ~UGameplayAbility

	/**
	 * Called when this ability is granted to the ability system component.
	 * Can be implemented in Blueprints.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityAdded")
	void K2_OnAbilityGiven();

	/**
	 * Called when this ability is removed from the ability system component.
	 * Can be implemented in Blueprints.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability", DisplayName = "OnAbilityRemoved")
	void K2_OnAbilityRemoved();

	/** Defines how the ability is supposed to be activated. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation")
	EAbilityActivationPolicy ActivationPolicy;

	/** Tries to activate the ability when it spawns. */
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
};
