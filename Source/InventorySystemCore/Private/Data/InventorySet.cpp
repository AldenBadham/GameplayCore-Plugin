// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/InventorySet.h"

#include "Components/InventorySystemComponent.h"
#include "Data/InventorySet_ItemSet.h"
#include "GameplayTags/InventoryGameplayTags.h"
#include "Log/InventorySystemLog.h"

FInventoryResult UInventorySet::GiveToInventorySystem(UInventorySystemComponent* InventorySystemComp)
{
	FInventoryResult Result;

	if (!IsValid(InventorySystemComp))
	{
		UE_LOG(LogInventorySystem, Error, TEXT("Tried to give InventorySet [%s] to an invalid InventorySystemComponent"), *GetFName().ToString());
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidComponent;
		return Result;
	}

	for (auto& [ItemDefinition, Quantity] : Items)
	{
		if (!IsValid(ItemDefinition) || Quantity <= 0)
		{
			UE_LOG(LogInventorySystem, Error, TEXT("Tried to give an invalid item [%s] or with a invalid quantity [%d] in the InventorySet [%s]"), *GetNameSafe(ItemDefinition), Quantity, *GetFName().ToString());
			Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidCount;
			continue;
		}

		return InventorySystemComp->TryAddItemDefinitionIn(TargetContainer, ItemDefinition, Quantity);
	}
	return Result;
}
