// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/EquipmentEntry.h"

#include "Components/EquipmentSystemComponent.h"
#include "Data/EquipmentList.h"
#include "Definitions/EquipmentDefinition.h"
#include "Instances/EquipmentInstance.h"
#include "Log/EquipmentSystemLog.h"

void FEquipmentEntry::PreReplicatedRemove(const FEquipmentList& InArraySerializer) const
{
	if (InArraySerializer.OwnerComponent)
	{
		UE_LOG(LogEquipmentSystem, Verbose, TEXT("%s: OnRemoveEntry (Non-Auth): %s (Last) %s"), *GetNameSafe(InArraySerializer.OwnerComponent->GetOwner()), *GetNameSafe(Instance), *GetNameSafe(LastInstance.Get()));
	}
}

void FEquipmentEntry::PostReplicatedAdd(const FEquipmentList& InArraySerializer) const
{
	if (InArraySerializer.OwnerComponent)
	{
		UE_LOG(LogEquipmentSystem, Verbose, TEXT("%s: OnAddedEntry (Non-Auth): %s (Last) %s"), *GetNameSafe(InArraySerializer.OwnerComponent->GetOwner()), *GetNameSafe(Instance), *GetNameSafe(LastInstance.Get()));
	}
}

void FEquipmentEntry::PostReplicatedChange(const FEquipmentList& InArraySerializer) const
{
	if (InArraySerializer.OwnerComponent)
	{
		UE_LOG(LogEquipmentSystem, Verbose, TEXT("%s: OnChangedEntry (Non-Auth): %s (Last) %s"), *GetNameSafe(InArraySerializer.OwnerComponent->GetOwner()), *GetNameSafe(Instance), *GetNameSafe(LastInstance.Get()));
	}
}

FString FEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s [Def: %s]"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}
