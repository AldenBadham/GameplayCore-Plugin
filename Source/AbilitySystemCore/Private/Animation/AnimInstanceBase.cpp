// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Animation/AnimInstanceBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Misc/DataValidation.h"

UAnimInstanceBase::UAnimInstanceBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
	GameplayTagPropertyMap.Initialize(this, nullptr);
}

void UAnimInstanceBase::InitializeWithAbilitySystem(UAbilitySystemComponent* AbilitySystemComp)
{
	check(AbilitySystemComp);
	GameplayTagPropertyMap.Initialize(this, AbilitySystemComp);
}

void UAnimInstanceBase::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (const AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* AbilitySystemComp = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor); IsValid(AbilitySystemComp))
		{
			InitializeWithAbilitySystem(AbilitySystemComp);
		}
	}
}

#if WITH_EDITOR
EDataValidationResult UAnimInstanceBase::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
