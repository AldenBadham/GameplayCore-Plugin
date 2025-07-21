// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Instances/Components/ItemComponent.h"


void UItemComponent::Initialize(UItemInstance& InInstance, UItemFragment* InSourceFragment)
{
	OwningInstance = &InInstance;
	SourceFragment = InSourceFragment;
}

UItemInstance* UItemComponent::GetOwningInstance()
{
	return OwningInstance;
}