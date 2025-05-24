// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

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
