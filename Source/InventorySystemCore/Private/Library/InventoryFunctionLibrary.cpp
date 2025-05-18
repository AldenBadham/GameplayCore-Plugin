// Fill out your copyright notice in the Description page of Project Settings.

#include "Library/InventoryFunctionLibrary.h"

#include "Definitions/Fragments/ItemFragment.h"
#include "Definitions/ItemDefinition.h"

const UItemFragment* UInventoryFunctionLibrary::FindItemDefinitionFragment(const TSubclassOf<UItemDefinition> ItemDef, const TSubclassOf<UItemFragment> FragmentClass)
{
	if (IsValid(ItemDef) && IsValid(FragmentClass))
	{
		return GetDefault<UItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
