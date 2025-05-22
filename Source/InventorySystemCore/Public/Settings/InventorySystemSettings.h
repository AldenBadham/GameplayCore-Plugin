#pragma once

#include "CoreMinimal.h"
#include "Definitions/Fragments/ItemFragment.h"
#include "Engine/DeveloperSettings.h"
#include "Settings/ItemFragmentRule.h"
#include "InventorySystemSettings.generated.h"

/**
 *
 */
UCLASS(config = "GameplayCoreSettings", DefaultConfig, DisplayName = "Inventory")
class INVENTORYSYSTEMCORE_API UInventorySystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// UDeveloperSettings
	virtual FName GetCategoryName() const override { return TEXT("Gameplay"); }
	// ~UDeveloperSettings

	#if WITH_EDITOR
	// Try to find the matching rules for the given fragment class
	const FItemFragmentRule* FindRuleForClass(const TSubclassOf<UItemFragment>& FragmentClass) const;
	#endif

	#if WITH_EDITORONLY_DATA
	// List of rules between fragments
	UPROPERTY(config, EditAnywhere, Category = "Fragment Rules")
	TMap<TSubclassOf<UItemFragment>, FItemFragmentRule> FragmentRules;
	#endif

	// TODO : Add item categories
};