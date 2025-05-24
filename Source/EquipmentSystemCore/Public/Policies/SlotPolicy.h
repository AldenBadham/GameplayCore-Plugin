// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SlotPolicy.generated.h"

class UItemInstance;
/**
 * Politique de slot d’équipement - règle de validation métier.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class EQUIPMENTSYSTEMCORE_API USlotPolicy : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Policy")
	bool bBlocksAllOtherSlots = false;

	UPROPERTY(EditDefaultsOnly, Category="Policy", Meta = (Categories = "Slot"))
	FGameplayTagContainer BlockedSlotTags;

	UFUNCTION(BlueprintNativeEvent, Category = "Policy")
	bool DoesBlockTag(const FGameplayTag& TargetSlot, const UItemInstance* Item = nullptr, const UObject* Context = nullptr) const;

	virtual bool CanEquipItem(const UItemInstance* Item, const FGameplayTag& TargetSlot, const UObject* Context) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Policy")
	bool K2_CanEquipItem(const UItemInstance* Item, const FGameplayTag& TargetSlot, const UObject* Context) const;

	UFUNCTION(BlueprintCallable, Category = "Policy")
	const FGameplayTagContainer& GetBlockedSlotTags() const;

	void GetBlockedSlotTags(FGameplayTagContainer& TagContainer) const;
};
