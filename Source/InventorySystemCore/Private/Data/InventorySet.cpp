// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/InventorySet.h"

#include "Components/InventorySystemComponent.h"
#include "Data/InventorySet_ItemSet.h"
#include "Log/InventorySystemLog.h"

void UInventorySet::GiveToInventorySystem(UInventorySystemComponent* InventorySystemComp)
{
	if (!IsValid(InventorySystemComp))
	{
		UE_LOG(LogInventorySystem, Error, TEXT("Tried to give InventorySet [%s] to an invalid InventorySystemComponent"), *GetFName().ToString());
	}

	for (auto& [ItemDefinition, Quantity] : Items)
	{
		if (!IsValid(ItemDefinition) || Quantity <= 0)
		{
			UE_LOG(LogInventorySystem, Error, TEXT("Tried to give an invalid item [%s] or with a invalid quantity [%d] in the InventorySet [%s]"), *GetNameSafe(ItemDefinition), Quantity, *GetFName().ToString());
			continue;
		}

		InventorySystemComp->AddItemDefinition(ItemDefinition, Quantity);
	}
}
