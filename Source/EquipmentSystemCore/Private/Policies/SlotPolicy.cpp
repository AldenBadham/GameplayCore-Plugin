// Fill out your copyright notice in the Description page of Project Settings.


#include "Policies/SlotPolicy.h"

#include "Components/EquipmentSystemComponent.h"

bool USlotPolicy::DoesBlockTag_Implementation(const FGameplayTag& TargetSlot, const UItemInstance* Item, const UObject* Context) const
{
	if (!BlockedSlotTags.IsValid())
	{
		return true;
	}

	return BlockedSlotTags.HasTag(TargetSlot) == false;
}

void USlotPolicy::GetBlockedSlotTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.Reset();
	TagContainer.AppendTags(BlockedSlotTags);
}

bool USlotPolicy::CanEquipItem(const UItemInstance* Item, const FGameplayTag& TargetSlot, const UObject* Context) const
{
	return K2_CanEquipItem(Item, TargetSlot, Context);
}

const FGameplayTagContainer& USlotPolicy::GetBlockedSlotTags() const
{
	return BlockedSlotTags;
}
