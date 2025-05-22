// Copyright 2025 TARA Gaming Limited. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ItemFragmentRule.generated.h"

class UItemFragment;


/**
 * @struct FItemFragmentRule
 * @see UItemFragment
 * @brief Defines rules for validating item fragments in the inventory system.
 * This structure allows specifying blocked and required fragments for validation purposes.
 * It can be used to enforce rules on item composition within the inventory system.
 */
USTRUCT(BlueprintType)
struct FItemFragmentRule
{
	GENERATED_BODY()

	/**
		 * Checks whether the rule is satisfied based on the provided fragment classes.
		 * @param FragmentClasses The array of fragment classes to validate against the rule.
		 * @param ErrorContext A string that will contain details about blocking or missing fragments if the rule is not
		 * satisfied for error context message.
		 * @return True if the rule is satisfied, false otherwise.
		 */
	bool IsRuleSatisfied(const TArray<TSubclassOf<UItemFragment>>& FragmentClasses, FString& ErrorContext) const;

	/**
	 * List of fragment classes that are blocked (cannot coexist with this fragment).
	 */
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UItemFragment>> BlockedFragments;

	/**
	 * List of fragment classes that are required (must be present for this fragment to be valid).
	 */
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UItemFragment>> RequiredFragments;
};