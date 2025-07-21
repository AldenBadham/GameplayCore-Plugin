// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SlotPolicy.generated.h"

class UItemInstance;
/**
 * USlotPolicy
 * 
 * Equipment slot policy, business validation rule.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class EQUIPMENTSYSTEMCORE_API USlotPolicy : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Policy")
	bool DoesBlockTag(const FGameplayTag& TargetSlot, const UItemInstance* Item = nullptr, const UObject* Context = nullptr) const;

	virtual bool CanEquipItem(const UItemInstance* Item, const FGameplayTag& TargetSlot, const UObject* Context) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Policy")
	bool K2_CanEquipItem(const UItemInstance* Item, const FGameplayTag& TargetSlot, const UObject* Context) const;

	UFUNCTION(BlueprintCallable, Category = "Policy")
	const FGameplayTagContainer& GetBlockedSlotTags() const;

	void GetBlockedSlotTags(FGameplayTagContainer& TagContainer) const;

public:
	
	UPROPERTY(EditDefaultsOnly, Category="Policy")
	bool bBlocksAllOtherSlots = false;

	UPROPERTY(EditDefaultsOnly, Category="Policy", Meta = (Categories = "Equipment.Slot"))
	FGameplayTagContainer BlockedSlotTags;
};
