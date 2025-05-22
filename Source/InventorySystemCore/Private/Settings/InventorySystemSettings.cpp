#include "Settings/InventorySystemSettings.h"
#include "Settings/ItemFragmentRule.h"

#if WITH_EDITOR
const FItemFragmentRule* UInventorySystemSettings::FindRuleForClass(const TSubclassOf<UItemFragment>& FragmentClass) const
{
	return FragmentRules.Find(FragmentClass);
}
#endif