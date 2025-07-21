// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "ItemComponent.h"
#include "ItemComponent_Consumable.generated.h"

struct FGameplayEffectContextHandle;
class UGameplayEffect;
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

	friend class UItemFragment_Consumable;

public:
	
	// UObject
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ~UObject

	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool CanConsume(const int32 UseCount = 1) const;

	UFUNCTION(BlueprintCallable)
	virtual void Consume(const int32 UseCount = 1);

	UFUNCTION(BlueprintCallable)
	virtual void SetRemainingUses(const int32 Count = 1);

	UFUNCTION(BlueprintCallable)
	virtual void RestoreUses();
	
protected:
	

	/** Tracks the current usage progress of the consumable item. 0 means unused, higher values indicate usage progress */
	UPROPERTY(Replicated)
	int32 RemainingUses = 0;

	UPROPERTY(Transient)
	int32 MaxUseCount = 0;
	
	UPROPERTY(Transient)
	TSubclassOf<UGameplayEffect> GameplayEffect;
	
	bool ShouldStackEffects = false;
};
