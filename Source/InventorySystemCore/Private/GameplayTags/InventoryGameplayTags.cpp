// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "GameplayTags/InventoryGameplayTags.h"

namespace InventorySystemGameplayTags
{
	// Inventory failure tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure, "Inventory.Failure", "Root tag for inventory failures");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_NullDefinition, "Inventory.Failure.NullDefinition", "The item to equip is null");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidCount, "Inventory.Failure.InvalidCount", "Zero or negative quantity given");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_NullInstance, "Inventory.Failure.NullItem", "The given item instance is invalid");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_NotAuthority, "Inventory.Failure.NullDefinition", "The item to equip is null");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_NotStorable, "Inventory.Failure.NotStorable", "The item do not own UItemFragment_Storable fragment");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_Uniqueness, "Inventory.Failure.Uniqueness", "An instance of the unique item already exist");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_DefinitionRefused, "Inventory.Failure.DefinitionRefused", "Item definition conditions are not met");	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_Internal, "Inventory.Failure.Internal", "Internal error during inventory process");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidComponent, "Inventory.Failure.InvalidComponent", "Invalid InventorySystemComponent");
	
} // namespace InventorySystemGameplayTags
