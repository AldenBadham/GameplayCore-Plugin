﻿// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "ItemFragment.h"

#include "ItemFragment_Consumable.generated.h"

class UGameplayEffect;

/**
 * @class UItemFragment_Consumable
 * @see UItemFragment
 * @brief Fragment that adds consumable behavior to an item
 * @details Allows items to be consumed and apply gameplay effects to the user. 
 * The fragment can be configured with multiple effects and a maximum number of uses.
 */
UCLASS(DisplayName = "Consumable Fragment")
class INVENTORYSYSTEMCORE_API UItemFragment_Consumable : public UItemFragment
{
	GENERATED_BODY()

public:
	/**
	 * Initializes the consumable fragment when the item instance is created
	 * @param Instance The item instance that owns this fragment
	 */
	virtual void OnInstanceCreated(UItemInstance* Instance) override;

	/**
	 * List of gameplay effects to apply when the item is consumed
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable")
	TArray<TSubclassOf<UGameplayEffect>> Effects = {};

	/**
	 * Maximum number of times this item can be consumed
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consumable", meta = (ClampMin = 1))
	int32 MaxUsesCount = 1;
};
