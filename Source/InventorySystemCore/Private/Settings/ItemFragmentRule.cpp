// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Settings/ItemFragmentRule.h"

#include "Definitions/Fragments/ItemFragment.h"

bool FItemFragmentRule::IsRuleSatisfied(const TArray<TSubclassOf<UItemFragment>>& FragmentClasses, FString& ErrorContext) const
{
	bool bIsSatisfied = true;
	TArray<FString> BlockingClasses;
	TArray<FString> MissingClasses;

	for (TSubclassOf FragmentClass : FragmentClasses)
	{
		if (BlockedFragments.Contains(FragmentClass))
		{
			bIsSatisfied = false;
			BlockingClasses.Add(FragmentClass->GetName());
		}
	}

	for (TSubclassOf FragmentClass : RequiredFragments)
	{
		if (!FragmentClasses.Contains(FragmentClass))
		{
			bIsSatisfied = false;
			MissingClasses.Add(FragmentClass->GetName());
		}
	}

	const FString BlockingClassesString = FString::Join(BlockingClasses, TEXT(", "));
	const FString MissingClassesString = FString::Join(MissingClasses, TEXT(", "));

	ErrorContext = FString::Printf(TEXT("Blocking fragment classes : %s - Missing fragment classes : %s"), *BlockingClassesString, *MissingClassesString);

	return bIsSatisfied;
}
