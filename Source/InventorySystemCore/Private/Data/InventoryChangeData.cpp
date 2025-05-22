// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/InventoryChangeData.h"

#include "Data/InventoryEntry.h"

FInventoryChangeData::FInventoryChangeData(const int32 InIndex, const FInventoryEntry& Entry, const EInventoryChangeType InChangeType)
{
	Index = InIndex;
	Instance = Entry.Instance;
	OldCount = Entry.LastStackCount;
	NewCount = Entry.StackCount;
	ChangeType = InChangeType;
}