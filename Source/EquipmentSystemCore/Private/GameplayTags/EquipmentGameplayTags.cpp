// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "GameplayTags/EquipmentGameplayTags.h"

namespace EquipmentSystemGameplayTags
{
	// Base tag for equipment slots
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Slot, "Equipment.Slot", "Root gameplay tag for equipment slots");

	// Equipment failure tags
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Failure, "Equipment.Failure", "Root tag for equipment failures");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Failure_NullItem, "Equipment.Failure.NullItem", "The item to equip is null");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Failure_MissingDefinition, "Equipment.Failure.MissingDefinition", "Equipment definition is missing");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Failure_SlotNotFound, "Equipment.Failure.SlotNotFound", "Requested slot does not exist");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Failure_SlotEmpty, "Equipment.Failure.SlotNotFound", "Requested slot does not exist");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Failure_SlotBlocked, "Equipment.Failure.SlotBlocked", "The slot is currently blocked");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Failure_PolicyRefused, "Equipment.Failure.PolicyRefused", "Equipment policy refused the equipment");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TAG_Equipment_Failure_Internal, "Equipment.Failure.Internal", "Internal error during equipment process");
} // namespace EquipmentSystemGameplayTags
