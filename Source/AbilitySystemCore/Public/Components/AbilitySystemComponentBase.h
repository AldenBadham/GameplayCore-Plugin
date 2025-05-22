#pragma once

#include "AbilitySystemComponent.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "AbilitySystemComponentBase.generated.h"

/**
 * @class UAbilitySystemComponentBase
 * @see UAbilitySystemComponent
 * @brief This class extends the base UAbilitySystemComponent to provide additional functionality.
 */
UCLASS(BlueprintType, ClassGroup = "Abilities", meta = (BlueprintSpawnableComponent))
class ABILITYSYSTEMCORE_API UAbilitySystemComponentBase : public UAbilitySystemComponent
{
	GENERATED_BODY()

	friend class UGameplayAbilityBase;

public:
	UAbilitySystemComponentBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UActorComponent
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ~UActorComponent

	// UAbilitySystemComponent
	/**
	 *	Initialized the Abilities' ActorInfo - the structure that holds information about who we are acting on and who controls us.
	 *      OwnerActor is the actor that logically owns this component.
	 *		AvatarActor is what physical actor in the world we are acting on. Usually a Pawn but it could be a Tower, Building, Turret, etc, may be the same as Owner
	 */
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	/** Called from the ability to let the component know it is ended */
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
	// ~UAbilitySystemComponent

	/**
	 * Handles the pressing of an ability input tag.
	 * @param InputTag The input tag that was pressed.
	 */
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	/**
	 * Handles the release of an ability input tag.
	 * @param InputTag The input tag that was released.
	 */
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/**
	 * Processes ability input.
	 * @param DeltaTime The time since the last frame.
	 * @param bGamePaused Whether the game is paused.
	 */
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);

	/** Clears all ability input. */
	void ClearAbilityInput();

protected:
	/** Initializes abilities when a new actor info is set. */
	void InitAbilitiesOnNewActorInfo();

	/** Tries to activate abilities on spawn. Applicable if the activation policy of the ability is EAbilityActivationPolicy::OnSpawn*/
	void TryActivateAbilitiesOnSpawn();

	/**
	 * Handles when an ability spec input is pressed.
	 * @param Spec The ability spec that had its input pressed.
	 */
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	/**
	 * Handles when an ability spec input is released.
	 * @param Spec The ability spec that had its input released.
	 */
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	/** Handles to abilities that had their input pressed this frame. */
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	/** Handles to abilities that had their input released this frame. */
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	/** Handles to abilities that have their input held. */
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};