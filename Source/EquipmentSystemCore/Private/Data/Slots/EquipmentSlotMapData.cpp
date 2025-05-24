// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/Slots/EquipmentSlotMapData.h"

FPrimaryAssetId UEquipmentSlotMapData::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("SlotMap", GetFName());
}