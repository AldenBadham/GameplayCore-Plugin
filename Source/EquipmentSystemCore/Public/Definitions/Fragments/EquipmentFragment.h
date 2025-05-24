// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "EquipmentFragment.generated.h"

class UEquipmentInstance;

/**
 * @class UEquipmentFragment
 * @see UEquipmentDefinition
 * @brief Represents a fragment of an equipment definition, allow definition extension.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType, Abstract)
class EQUIPMENTSYSTEMCORE_API UEquipmentFragment : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Called when an equipment instance is created.
	 * @param Instance The equipment instance associated with this fragment.
	 */
	virtual void OnInstanceCreated(UEquipmentInstance* Instance) const;
};
