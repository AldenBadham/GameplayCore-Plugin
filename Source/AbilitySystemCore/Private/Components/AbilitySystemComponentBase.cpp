// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/AbilitySystemComponentBase.h"

#include "Abilities/AbilityActivationPolicy.h"
#include "Abilities/GameplayAbilityBase.h"
#include "AbilitySystemCoreTags.h"

UAbilitySystemComponentBase::UAbilitySystemComponentBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
}

void UAbilitySystemComponentBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UAbilitySystemComponentBase::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	if (!IsValid(InOwnerActor))
	{
		return;
	}

	const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	if (!ActorInfo)
	{
		return;
	}

	// Check if we have a new pawn avatar
	const bool bHasNewAvatar = Cast<APawn>(InAvatarActor) && InAvatarActor != ActorInfo->AvatarActor;
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	// If we get a new pawn as avatar actor, initialize new actor info
	if (bHasNewAvatar)
	{
		InitAbilitiesOnNewActorInfo();
	}
}

void UAbilitySystemComponentBase::NotifyAbilityEnded(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

void UAbilitySystemComponentBase::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UAbilitySystemComponentBase::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UAbilitySystemComponentBase::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	// Check if abilities input activation is blocked
	if (HasMatchingGameplayTag(AbilitySystemCoreTags::TAG_Gameplay_AbilitiesBlocked))
	{
		ClearAbilityInput();
		return;
	}

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	// Process all abilities that activate when the input is held.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UGameplayAbilityBase* BaseAbilityCDO = Cast<UGameplayAbilityBase>(AbilitySpec->Ability);
				if (BaseAbilityCDO && BaseAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// Process all abilities that had their input pressed this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UGameplayAbilityBase* BaseAbilityCDO = Cast<UGameplayAbilityBase>(AbilitySpec->Ability);

					if (BaseAbilityCDO && BaseAbilityCDO->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// Process all abilities that had their input released this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	// Clear the cached ability handles.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UAbilitySystemComponentBase::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void UAbilitySystemComponentBase::InitAbilitiesOnNewActorInfo()
{
	if ([[maybe_unused]] const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get())
	{
		TryActivateAbilitiesOnSpawn();
	}
}

void UAbilitySystemComponentBase::TryActivateAbilitiesOnSpawn()
{
	// Stop from removing ability from the ability system to iterate through the abilities
	ABILITYLIST_SCOPE_LOCK();

	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const UGameplayAbilityBase* Ability = CastChecked<UGameplayAbilityBase>(AbilitySpec.Ability);
		Ability->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}

void UAbilitySystemComponentBase::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// UE : We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		FPredictionKey OriginalPredictionKey;
		if (const UGameplayAbility* Instance = Spec.GetPrimaryInstance())
		{
			OriginalPredictionKey = Instance->GetCurrentActivationInfo().GetActivationPredictionKey();
		}

		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, OriginalPredictionKey);
	}
}

void UAbilitySystemComponentBase::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		FPredictionKey OriginalPredictionKey;
		if (const UGameplayAbility* Instance = Spec.GetPrimaryInstance())
		{
			OriginalPredictionKey = Instance->GetCurrentActivationInfo().GetActivationPredictionKey();
		}

		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, OriginalPredictionKey);
	}
}
