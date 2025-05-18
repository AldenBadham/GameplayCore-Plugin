// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/ItemFragmentRule.h"
#include "Settings/InventorySystemSettings.h"

#if WITH_EDITOR
const FItemFragmentRule* UInventorySystemSettings::FindRuleForClass(const TSubclassOf<UItemFragment>& FragmentClass) const
{
	return FragmentRules.Find(FragmentClass);
}
#endif