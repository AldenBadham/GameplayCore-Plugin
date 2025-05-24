// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Animation/AnimInstanceBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Misc/DataValidation.h"

UAnimInstanceBase::UAnimInstanceBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
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

float UAnimInstanceBase::CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation)
{
	if (!Velocity.IsNearlyZero())
	{
		const FMatrix RotMatrix = FRotationMatrix(BaseRotation);
		const FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
		const FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
		const FVector NormalizedVel = Velocity.GetSafeNormal2D();

		// get a cos(alpha) of forward vector vs velocity
		const float ForwardCosAngle = static_cast<float>(FVector::DotProduct(ForwardVector, NormalizedVel));
		// now get the alpha and convert to degree
		float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

		// depending on where right vector is, flip it
		const float RightCosAngle = static_cast<float>(FVector::DotProduct(RightVector, NormalizedVel));
		if (RightCosAngle < 0.f)
		{
			ForwardDeltaDegree *= -1.f;
		}

		return ForwardDeltaDegree;
	}

	return 0.f;
}

#if WITH_EDITOR
EDataValidationResult UAnimInstanceBase::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
