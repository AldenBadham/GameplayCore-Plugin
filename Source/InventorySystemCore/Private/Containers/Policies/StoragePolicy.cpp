// Licensed under the MIT License. See the LICENSE file in the project root for full license information.


#include "Containers/Policies/StoragePolicy.h"

bool UStoragePolicy::CanStoreItem_Implementation(UItemInstance* Instance, FGameplayTag& OutFailureReason) const
{
	return true;
}
