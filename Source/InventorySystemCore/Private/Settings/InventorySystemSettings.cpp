// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Settings/InventorySystemSettings.h"

#include "Settings/ItemFragmentRule.h"

#if WITH_EDITOR
const FItemFragmentRule* UInventorySystemSettings::FindRuleForClass(const TSubclassOf<UItemFragment>& FragmentClass) const
{
	return FragmentRules.Find(FragmentClass);
}
#endif
