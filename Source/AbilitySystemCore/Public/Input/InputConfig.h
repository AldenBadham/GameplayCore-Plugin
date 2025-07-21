// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"

#include "InputConfig.generated.h"

class UInputAction;
struct FInputTagSet;

/**
 * UInputConfig
 *
 *	Non-mutable data asset that contains input configuration properties.
 */
UCLASS(BlueprintType, Const)
class ABILITYSYSTEMCORE_API UInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Input")
	const UInputAction* FindInputActionForTag(const FGameplayTag& Tag, bool bLogNotFound = true) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (TitleProperty = "InputAction"))
	TArray<FInputTagSet> InputTagSets;
};
