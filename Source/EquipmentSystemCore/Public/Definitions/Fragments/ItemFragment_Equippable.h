// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Definitions/Fragments/ItemFragment.h"
#include "ItemFragment_Equippable.generated.h"

class UEquipmentSystemComponent;
class UEquipmentDefinition;

/**
 *
 */
UCLASS(DisplayName = "Equipable")
class EQUIPMENTSYSTEMCORE_API UItemFragment_EquipableItem : public UItemFragment
{
	GENERATED_BODY()

public:
	/**
	 * Called when an item instance is created. This method initializes any equipable-specific functionality for the item instance.
	 * @param Instance The item instance associated with this fragment.
	 */
	virtual void OnInstanceCreated(UItemInstance* Instance) override;

	/**
	 * The equipment definition associated with this fragment.
	 * @note This defines the properties and behavior of the equipped item.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipable")
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;
};