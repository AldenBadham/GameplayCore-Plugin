// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Stats/Stats.h"

DECLARE_STATS_GROUP(TEXT("EquipmentSystem"), STATGROUP_EquipmentSystem, STATCAT_Advanced);

DECLARE_CYCLE_STAT_EXTERN(TEXT("Equipment - Internal_ProcessEquip"), STAT_Equipment_ProcessEquip, STATGROUP_EquipmentSystem,);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Equipment - Internal_ProcessUnequip"), STAT_Equipment_ProcessUnequip, STATGROUP_EquipmentSystem,);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Equipment - Internal_EquipOnSlot"), STAT_Equipment_EquipOnSlot, STATGROUP_EquipmentSystem,);
