#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "InputTagSet.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct ABILITYSYSTEMCORE_API FInputTagSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};