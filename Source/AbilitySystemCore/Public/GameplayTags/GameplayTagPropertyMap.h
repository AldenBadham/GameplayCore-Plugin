// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "GameplayTagPropertyMap.generated.h"

/**
 * @class FGameplayTagPropertyMap
 *
 * @brief Struct used to manage gameplay tag blueprint property mappings.
 * It registers the properties with delegates on an ability system component.
 * This struct can not be used in containers (such as TArray) since it uses a raw pointer
 * to bind the delegate and it's address could change causing an invalid binding.
 */
USTRUCT()
struct ABILITYSYSTEMCORE_API FGameplayTagPropertyMap
{
	GENERATED_BODY()

	FGameplayTagPropertyMap();
	FGameplayTagPropertyMap(const FGameplayTagPropertyMap& Other);
	~FGameplayTagPropertyMap();

	/** Call this to initialize and bind the properties with the ability system component. */
	void Initialize(UObject* Owner, UAbilitySystemComponent* AbilitySystemComp);

	/** Call to manually apply the current tag state, can handle cases where callbacks were skipped */
	void ApplyCurrentTags();

	/** Bind the give class property (bool, int or float) to the GameplayTag.
	 * @param Tag Gameplay tag to listen
	 * @param PropertyName FName of the property to bind. Use GET_MEMBER_NAME_CHECKED macro for that
	 */
	void AddTag(const FGameplayTag& Tag, const FName& PropertyName);

#if WITH_EDITOR
	/** This can optionally be called in the owner's IsDataValid() for data validation. */
	EDataValidationResult IsDataValid(const UObject* ContainingAsset, FDataValidationContext& Context) const;
#endif

protected:
	void Unregister();

	void GameplayTagEventCallback(const FGameplayTag Tag, int32 NewCount, TWeakObjectPtr<UObject> RegisteredOwner);

	static bool IsPropertyTypeValid(const FProperty* Property);

	static EGameplayTagEventType::Type GetGameplayTagEventType(const FProperty* Property);

	TWeakObjectPtr<UObject> CachedOwner;
	TWeakObjectPtr<UAbilitySystemComponent> CachedAbilitySystemComponent;

	UPROPERTY(EditAnywhere, Category = GameplayTagBlueprintProperty)
	TArray<FGameplayTagBlueprintPropertyMapping> PropertyMappings;
};
