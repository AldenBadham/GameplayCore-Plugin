// Licensed under the MIT License. See the LICENSE file in the project root for full license information.


#include "Definitions/Fragments/ItemFragment_TagModifier.h"

void UItemFragment_TagModifier::OnInstanceCreated(UItemInstance* Instance)
{
	Super::OnInstanceCreated(Instance);

	for (const FGameplayTag& Tag : TagsToAdd)
	{
		Instance->AddTag(Tag);
	}
	for (const FGameplayTag& Tag : TagsToRemove)
	{
		Instance->RemoveTag(Tag);
	}
}
