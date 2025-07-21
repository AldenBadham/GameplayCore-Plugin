// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "Interfaces/AbilityAnimInterface.h"
#include "GameplayTags/GameplayTagPropertyMap.h"
#include "AnimInstanceBase.generated.h"

/**
 *
 */
UCLASS()
class ABILITYSYSTEMCORE_API UAnimInstanceBase : public UAnimInstance, public IAbilityAnimInterface
{
	GENERATED_BODY()

public:
	UAnimInstanceBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* AbilitySystemComp) override;

protected:
	virtual void NativeInitializeAnimation() override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

	/* Gameplay tags that can be mapped to blueprint variables.
	 * The variables will automatically update as the tags are added or removed.
	 * These should be used instead of manually querying for the gameplay tags.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayTags")
	FGameplayTagPropertyMap GameplayTagPropertyMap;
};
