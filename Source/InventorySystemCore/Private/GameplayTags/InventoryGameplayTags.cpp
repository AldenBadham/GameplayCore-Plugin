// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "GameplayTags/InventoryGameplayTags.h"

namespace InventorySystemGameplayTags
{
	// Inventory container tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Container, "Inventory.Container", "Root tag for inventory containers");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Container_Default, "Inventory.Container.Default", "Default gameplay tag used for inventory container");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Container_Bag, "Inventory.Container.Bag", "Default gameplay tag used for inventory container");

	// Inventory failure tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure, "Inventory.Failure", "Root tag for inventory failures");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_ContainerNotFound, "Inventory.Failure.ContainerNotFound", "Root tag for inventory failures");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidDefinition, "Inventory.Failure.InvalidDefinition", "The item to equip is null");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidCount, "Inventory.Failure.InvalidCount", "Zero or negative quantity given");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidInstance, "Inventory.Failure.InvalidInstance", "The given item instance is invalid");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_NotAuthority, "Inventory.Failure.NotAuthority", "The item to equip is null");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_NotStorable, "Inventory.Failure.NotStorable", "The item do not own UItemFragment_Storable fragment");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_Uniqueness, "Inventory.Failure.Uniqueness", "An instance of the unique item already exist");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_DefinitionRefused, "Inventory.Failure.DefinitionRefused", "Item definition conditions are not met");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_Internal, "Inventory.Failure.Internal", "Internal error during inventory process");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidComponent, "Inventory.Failure.InvalidComponent", "Invalid InventorySystemComponent");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidContainer, "Inventory.Failure.InvalidContainer", "Invalid InventoryContainer");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidHandle, "Inventory.Failure.InvalidHandle", "Invalid inventory entry handle");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_HandleMismatch, "Inventory.Failure.HandleMismatch", "Invalid inventory entry handle");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Inventory_Failure_InvalidIndex, "Inventory.Failure.InvalidIndex", "Invalid inventory entry handle");
} // namespace InventorySystemGameplayTags
