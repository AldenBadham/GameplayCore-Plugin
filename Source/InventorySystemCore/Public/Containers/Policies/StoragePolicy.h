// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StoragePolicy.generated.h"

struct FGameplayTag;
class UItemInstance;
/**
 * @class UStoragePolicy
 * Base abstract class for container policies.
 * Implementations should define CanStoreItem and provide validation rules.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class INVENTORYSYSTEMCORE_API UStoragePolicy : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Determines whether the given item instance can be stored in the container.
	 * @param Instance - the item being considered
	 * @param OutFailureReason - if false, the reason the item is rejected
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Inventory|Policy")
	bool CanStoreItem(UItemInstance* Instance, FGameplayTag& OutFailureReason) const;
};
