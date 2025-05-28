// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "TestItemDefinition.h"
#include "TestItemDefinition_Unique.generated.h"

/**
 * @class UTestItemDefinition_Unique
 * @see UNQItemDefinition
 * This class of ItemDefinition is created for automation test only for an unique item.
 * /!\ SHOULD NOT USED FOR GAMEPLAY /!\
 */
UCLASS(Experimental, Hidden)

class INVENTORYSYSTEMEDITOR_API UTestItemDefinition_Unique : public UTestItemDefinition
{
	GENERATED_BODY()

public:
	UTestItemDefinition_Unique(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer.Get())
	{
		StorableFragment->MaxStackCount = 1;
		StorableFragment->StorageFlags |= static_cast<int32>(EItemStorageFlags::Unique);
	}
};
