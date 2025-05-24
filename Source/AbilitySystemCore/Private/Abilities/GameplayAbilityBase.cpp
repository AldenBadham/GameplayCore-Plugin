// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Abilities/GameplayAbilityBase.h"

#include "AbilitySystemComponent.h"
#include "Globals/AbilitySystemGlobalsBase.h"

UGameplayAbilityBase::UGameplayAbilityBase(const FObjectInitializer& ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
}

void UGameplayAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// OnAvatarSet being called by Super::OnGiveAbility if it has a valid AvatarActor
	// try to activate the ability
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UGameplayAbilityBase::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	// Call blueprint implementation after the ability is given
	K2_OnAbilityGiven();
}

void UGameplayAbilityBase::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	// Call blueprint implementation before the ability is removed.
	K2_OnAbilityRemoved();

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UGameplayAbilityBase::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UGameplayAbilityBase::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

bool UGameplayAbilityBase::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bBlocked = false;
	bool bMissing = false;

	const UAbilitySystemGlobalsBase& AbilitySystemGlobals = UAbilitySystemGlobalsBase::Get();
	const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
	const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

	// Check if any of this ability's tags are currently blocked
	if (AbilitySystemComponent.AreAbilityTagsBlocked(GetAssetTags()))
	{
		bBlocked = true;
	}

	// Check to see the required/blocked tags for this ability
	if (ActivationBlockedTags.Num() || ActivationRequiredTags.Num())
	{
		static FGameplayTagContainer AbilitySystemComponentTags;
		AbilitySystemComponentTags.Reset();

		AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);
		AbilitySystemComponent.GetBlockedAbilityTags(AbilitySystemComponentTags);

		if (AbilitySystemComponentTags.HasAny(ActivationBlockedTags))
		{
			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(ActivationRequiredTags))
		{
			bMissing = true;
		}
	}

	// Check source tags
	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	// Check target tags
	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				bMissing = true;
			}
		}
	}

	// We succeeded if there were no blocked tags and no missing required tags
	return !bBlocked && !bMissing;
}

void UGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const bool bReplicateEndAbility, const bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbilityBase::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = Spec.GetPrimaryInstance()->GetCurrentActivationInfo().ActivationMode == EGameplayAbilityActivationMode::Predicting;

	// Try to activate if activation policy is OnSpawn.
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && ActivationPolicy == EAbilityActivationPolicy::OnSpawn)
	{
		UAbilitySystemComponent* AbilitySystemComp = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		if (IsValid(AbilitySystemComp) && IsValid(AvatarActor))
		{
			// Don't try to activate the ability if the avatar is being destroyed and no longer replicated
			if (!AvatarActor->GetTearOff() && AvatarActor->GetLifeSpan() <= 0.0f)
			{
				// Activate the ability only if we're on the authority
				const bool bIsLocalExecution = NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted || NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly;
				const bool bIsServerExecution = NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly || NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated;

				const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
				const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

				if (bClientShouldActivate || bServerShouldActivate)
				{
					AbilitySystemComp->TryActivateAbility(Spec.Handle);
				}
			}
		}
	}
}
