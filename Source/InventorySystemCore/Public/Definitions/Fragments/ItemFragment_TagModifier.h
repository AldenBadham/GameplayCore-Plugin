// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemFragment.h"
#include "ItemFragment_TagModifier.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Storable Fragment")
class INVENTORYSYSTEMCORE_API UItemFragment_TagModifier : public UItemFragment
{
	GENERATED_BODY()

public:
	// UItemFragment
	virtual void OnInstanceCreated(UItemInstance* Instance) override;
	// ~UItemFragment

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer TagsToAdd;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer TagsToRemove;
};
