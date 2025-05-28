// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Definitions/ItemDefinition.h"
#include "Definitions/Fragments/ItemFragment_Storable.h"

#include "TestItemDefinition.generated.h"

/**
 * @class UTestItemDefinition
 * @see UItemDefinition
 * This class of ItemDefinition is created for automation test only.
 * /!\ SHOULD NOT USED FOR GAMEPLAY /!\
 */
UCLASS(Experimental, Hidden)
class INVENTORYSYSTEMEDITOR_API UTestItemDefinition : public UItemDefinition
{
	GENERATED_BODY()

public:
	UTestItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer.Get())
	{
		// Create and configure the default Storable fragment
		StorableFragment = CreateDefaultSubobject<UItemFragment_Storable>(TEXT("StorableFragment"));
		check(StorableFragment);

		StorableFragment->MaxStackCount = 10;
		Fragments.Add(StorableFragment);
	}

protected:
	UPROPERTY()
	UItemFragment_Storable* StorableFragment = nullptr;
};
