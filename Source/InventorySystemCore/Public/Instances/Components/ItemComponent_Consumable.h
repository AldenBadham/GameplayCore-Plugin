// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "ItemComponent.h"
#include "ItemComponent_Consumable.generated.h"

class UGameplayAbility;

/**
 * @class UItemComponent_Consumable
 * @see UItemComponent
 * @brief Component for items that can be consumed or used over time.
 * @details This component tracks the usage state of consumable items, such as 
 * potions, food, or other items that can be depleted. It maintains a replicated 
 * current use value that represents the item's consumption progress.
 */
UCLASS()
class INVENTORYSYSTEMCORE_API UItemComponent_Consumable : public UItemComponent
{
	GENERATED_BODY()

public:
	// UObject
	/**
	 * Sets up property replication for the consumable component
	 * @param OutLifetimeProps Array of properties to be replicated
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityClass;

	/** Tracks the current usage progress of the consumable item. 0 means unused, higher values indicate usage progress */
	UPROPERTY(Replicated)
	float CurrentUse = 0.0f;
};
