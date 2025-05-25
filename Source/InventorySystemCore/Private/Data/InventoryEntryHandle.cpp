// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Data/InventoryEntryHandle.h"

#include "Containers/InventoryContainer.h"

bool FInventoryEntryHandle::IsHandleValid() const
{
	return EntryIndex != INDEX_NONE && IsValid(Container);
}
