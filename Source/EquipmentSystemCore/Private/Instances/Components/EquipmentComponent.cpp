// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Instances/Components/EquipmentComponent.h"

void UEquipmentComponent::Initialize(UEquipmentInstance* InInstance)
{
	OwningInstance = InInstance;
}

UEquipmentInstance* UEquipmentComponent::GetOwningInstance()
{
	return OwningInstance;
}
