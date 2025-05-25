// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StoragePolicy.h"
#include "UObject/Object.h"
#include "StoragePolicy_TagRequirement.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Tag Requirements")
class INVENTORYSYSTEMCORE_API UStoragePolicy_TagRequirement : public UStoragePolicy
{
	GENERATED_BODY()

public:
	/** Required tag the item must have to be accepted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Policy")
	FGameplayTagContainer RequiredTags;

	/** Required tag the item must not have to be accepted */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Policy")
	FGameplayTagContainer ForbiddenTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Policy")
	bool bExactMatch = true;

	virtual bool CanStoreItem_Implementation(UItemInstance* Instance, FGameplayTag& OutFailureReason) const override;
};
