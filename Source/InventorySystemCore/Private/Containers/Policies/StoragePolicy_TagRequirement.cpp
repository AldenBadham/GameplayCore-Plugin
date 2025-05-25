// Licensed under the MIT License. See the LICENSE file in the project root for full license information.


#include "Containers/Policies/StoragePolicy_TagRequirement.h"

#include "Instances/ItemInstance.h"

bool UStoragePolicy_TagRequirement::CanStoreItem_Implementation(UItemInstance* Instance, FGameplayTag& OutFailureReason) const
{
	if (!Super::CanStoreItem_Implementation(Instance, OutFailureReason))
	{
		return false;
	}

	FGameplayTagContainer TagContainer;
	Instance->GetOwnedGameplayTags(TagContainer);

	if (bExactMatch && TagContainer.HasAllExact(RequiredTags) && !TagContainer.HasAnyExact(ForbiddenTags))
	{
		return true;
	}
	if (!bExactMatch && TagContainer.HasAny(RequiredTags) && !TagContainer.HasAny(ForbiddenTags))
	{
		return true;
	}

	return false;
}
