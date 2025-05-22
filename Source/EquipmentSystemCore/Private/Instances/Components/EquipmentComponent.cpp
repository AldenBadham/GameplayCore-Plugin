#include "Instances/Components/EquipmentComponent.h"

void UEquipmentComponent::Initialize(UEquipmentInstance* InInstance)
{
	OwningInstance = InInstance;
}

UEquipmentInstance* UEquipmentComponent::GetOwningInstance()
{
	return OwningInstance;
}