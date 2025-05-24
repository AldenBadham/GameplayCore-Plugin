// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ItemFragment.generated.h"

class UItemInstance;

/**
 * @class UItemFragment
 * @see UItemDefinition
 * @brief Represents a fragment of an item definition, allowing definition extension.
 * @details Item fragments are components that can be attached to item definitions to extend their functionality.
 * They are instantiated along with the item instance and can contain both data and behavior.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType, Abstract)
class INVENTORYSYSTEMCORE_API UItemFragment : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Called when the ItemInstance is created and this fragment is attached to it
	 * @param Instance The item instance that owns this fragment
	 */
	virtual void OnInstanceCreated(UItemInstance* Instance)
	{
	}
};
