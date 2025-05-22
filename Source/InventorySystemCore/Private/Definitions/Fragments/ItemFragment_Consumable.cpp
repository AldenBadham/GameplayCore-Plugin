// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Definitions/Fragments/ItemFragment_Consumable.h"
#include "Instances/ItemInstance.h"

#include "Instances/Components/ItemComponent_Consumable.h"
#include "Log/InventorySystemLog.h"

void UItemFragment_Consumable::OnInstanceCreated(UItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	if (const auto Component = Instance->AddComponent<UItemComponent_Consumable>(); IsValid(Component))
	{
		Component->CurrentUse = MaxUsesCount;
		Component->Initialize(Instance);
		return;
	}
	
	UE_LOG(LogInventorySystem, Warning, TEXT("Failed to add ItemComponent_Consumable to ItemInstance: %s"), *GetNameSafe(Instance));
}