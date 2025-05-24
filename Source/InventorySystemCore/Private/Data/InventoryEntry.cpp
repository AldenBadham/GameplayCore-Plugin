// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/InventoryEntry.h"

#include "Components/InventorySystemComponent.h"
#include "Data/InventoryList.h"
#include "Instances/ItemInstance.h"
#include "Log/InventorySystemLog.h"

void FInventoryEntry::PreReplicatedRemove(const FInventoryList& InArraySerializer) const
{
	if (InArraySerializer.OwnerComponent)
	{
		UE_LOG(LogInventorySystem, Verbose, TEXT("%s: OnRemoveEntry (Non-Auth): %s. Count: %d"), *GetNameSafe(InArraySerializer.OwnerComponent->GetOwner()), *GetNameSafe(Instance), StackCount);
	}
}

void FInventoryEntry::PostReplicatedAdd(const FInventoryList& InArraySerializer) const
{
	if (InArraySerializer.OwnerComponent)
	{
		UE_LOG(LogInventorySystem, Verbose, TEXT("%s: OnAddedEntry (Non-Auth): %s. Count: %d"), *GetNameSafe(InArraySerializer.OwnerComponent->GetOwner()), *GetNameSafe(Instance), StackCount);
	}
}

void FInventoryEntry::PostReplicatedChange(const FInventoryList& InArraySerializer) const
{
	if (InArraySerializer.OwnerComponent)
	{
		UE_LOG(LogInventorySystem, Verbose, TEXT("%s: OnAddedChanged (Non-Auth): %s. Count: %d   LastCount: %d"), *GetNameSafe(InArraySerializer.OwnerComponent->GetOwner()), *GetNameSafe(Instance), StackCount, LastStackCount);
	}
}

FString FInventoryEntry::GetDebugString() const
{
	return FString::Printf(TEXT("(%s)"), *GetNameSafe(Instance));
}
