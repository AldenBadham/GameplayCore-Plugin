// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "GameplayTags/GameplayTagPropertyMap.h"

#include "AbilitySystemComponent.h"
#include "Log/AbilitySystemLog.h"
#include "Misc/DataValidation.h"

FGameplayTagPropertyMap::FGameplayTagPropertyMap() {}

FGameplayTagPropertyMap::FGameplayTagPropertyMap(const FGameplayTagPropertyMap& Other)
{
	ensureMsgf(Other.CachedOwner.IsExplicitlyNull(), TEXT("FGameplayTagBlueprintPropertyMap cannot be used inside an array or other container that is copied after register!"));
	PropertyMappings = Other.PropertyMappings;
}

FGameplayTagPropertyMap::~FGameplayTagPropertyMap()
{
	Unregister();
}

void FGameplayTagPropertyMap::Initialize(UObject* Owner, UAbilitySystemComponent* AbilitySystemComp)
{
	const UClass* OwnerClass = Owner ? Owner->GetClass() : nullptr;
	if (!OwnerClass)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("FGameplayTagPropertyMap: Initialize() called with an invalid Owner."));
		return;
	}

	if (!AbilitySystemComp)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("FGameplayTagPropertyMap: Initialize() called with an invalid AbilitySystemComponent."));
		return;
	}

	if (CachedOwner == Owner && CachedAbilitySystemComponent == AbilitySystemComp)
	{
		// Already initialized.
		return;
	}

	if (CachedOwner.IsValid())
	{
		Unregister();
	}

	// Cache
	CachedOwner = Owner;
	CachedAbilitySystemComponent = AbilitySystemComp;

	const FOnGameplayEffectTagCountChanged::FDelegate Delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateRaw(this, &FGameplayTagPropertyMap::GameplayTagEventCallback, CachedOwner);

	// Process array starting at the end so we can remove invalid entries.
	for (int32 MappingIndex = PropertyMappings.Num() - 1; MappingIndex >= 0; --MappingIndex)
	{
		FGameplayTagBlueprintPropertyMapping& Mapping = PropertyMappings[MappingIndex];

		if (Mapping.TagToMap.IsValid())
		{
			FProperty* Property = OwnerClass->FindPropertyByName(Mapping.PropertyName);
			if (Property && IsPropertyTypeValid(Property))
			{
				Mapping.PropertyToEdit = Property;
				Mapping.DelegateHandle = AbilitySystemComp->RegisterAndCallGameplayTagEvent(Mapping.TagToMap, Delegate, GetGameplayTagEventType(Property));
				continue;
			}
		}

		// Entry was invalid.  Remove it from the array.
		UE_LOG(
			LogAbilitySystem,
			Error,
			TEXT("FGameplayTagPropertyMap: Removing invalid GameplayTagBlueprintPropertyMapping [Index: %d, Tag:%s, Property:%s] for [%s]."),
			MappingIndex,
			*Mapping.TagToMap.ToString(),
			*Mapping.PropertyName.ToString(),
			*GetNameSafe(Owner));

		PropertyMappings.RemoveAtSwap(MappingIndex, EAllowShrinking::No);
	}
}

void FGameplayTagPropertyMap::ApplyCurrentTags()
{
	UObject* Owner = CachedOwner.Get();
	if (!Owner)
	{
		UE_LOG(LogAbilitySystem, Warning, TEXT("FGameplayTagPropertyMap::ApplyCurrentTags called with an invalid Owner."));
		return;
	}

	const UAbilitySystemComponent* AbilitySystemComp = CachedAbilitySystemComponent.Get();
	if (!AbilitySystemComp)
	{
		UE_LOG(LogAbilitySystem, Warning, TEXT("FGameplayTagPropertyMap::ApplyCurrentTags called with an invalid AbilitySystemComponent."));
		return;
	}

	// Property mapping
	for (FGameplayTagBlueprintPropertyMapping& Mapping : PropertyMappings)
	{
		if (Mapping.PropertyToEdit.Get() && Mapping.TagToMap.IsValid())
		{
			int32 NewCount = AbilitySystemComp->GetTagCount(Mapping.TagToMap);

			if (const FBoolProperty* BoolProperty = CastField<const FBoolProperty>(Mapping.PropertyToEdit.Get()))
			{
				BoolProperty->SetPropertyValue_InContainer(Owner, NewCount > 0);
			}
			else if (const FIntProperty* IntProperty = CastField<const FIntProperty>(Mapping.PropertyToEdit.Get()))
			{
				IntProperty->SetPropertyValue_InContainer(Owner, NewCount);
			}
			else if (const FFloatProperty* FloatProperty = CastField<const FFloatProperty>(Mapping.PropertyToEdit.Get()))
			{
				FloatProperty->SetPropertyValue_InContainer(Owner, static_cast<float>(NewCount));
			}
		}
	}
}

void FGameplayTagPropertyMap::AddTag(const FGameplayTag& Tag, const FName& PropertyName)
{
	const UObject* Owner = CachedOwner.Get();
	if (!Owner)
	{
		UE_LOG(LogAbilitySystem, Warning, TEXT("FGameplayTagPropertyMap::AddTag called with an invalid Owner."));
		return;
	}

	const UClass* OwnerClass = Owner->GetClass();

	UAbilitySystemComponent* AbilitySystemComp = CachedAbilitySystemComponent.Get();
	if (!AbilitySystemComp)
	{
		UE_LOG(LogAbilitySystem, Warning, TEXT("FGameplayTagPropertyMap::AddTag called with an invalid AbilitySystemComponent."));
		return;
	}

	FProperty* Property = OwnerClass->FindPropertyByName(PropertyName);
	if (!Property || !IsPropertyTypeValid(Property))
	{
		UE_LOG(LogAbilitySystem, Warning, TEXT("FGameplayTagPropertyMap::AddTag called with an invalid property."));
		return;
	}

	if (!Tag.IsValid())
	{
		UE_LOG(LogAbilitySystem, Warning, TEXT("FGameplayTagPropertyMap::AddTag called with an invalid gameplay tag."));
		return;
	}

	const FOnGameplayEffectTagCountChanged::FDelegate Delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateRaw(this, &FGameplayTagPropertyMap::GameplayTagEventCallback, CachedOwner);

	FGameplayTagBlueprintPropertyMapping Mapping;
	Mapping.TagToMap = Tag;
	Mapping.PropertyToEdit = Property;
	Mapping.PropertyName = PropertyName;
	Mapping.DelegateHandle = AbilitySystemComp->RegisterAndCallGameplayTagEvent(Tag, Delegate, GetGameplayTagEventType(Property));

	// Add the mapping to the map to be able to apply it manually
	PropertyMappings.Add(Mapping);
}

void FGameplayTagPropertyMap::Unregister()
{
	// Unregister all delegates for each gameplay tags
	if (UAbilitySystemComponent* AbilitySystemComp = CachedAbilitySystemComponent.Get())
	{
		for (FGameplayTagBlueprintPropertyMapping& Mapping : PropertyMappings)
		{
			if (Mapping.PropertyToEdit.Get() && Mapping.TagToMap.IsValid())
			{
				AbilitySystemComp->UnregisterGameplayTagEvent(Mapping.DelegateHandle, Mapping.TagToMap, GetGameplayTagEventType(Mapping.PropertyToEdit.Get()));
			}

			Mapping.PropertyToEdit = nullptr;
			Mapping.DelegateHandle.Reset();
		}
	}

	CachedOwner = nullptr;
	CachedAbilitySystemComponent = nullptr;
}

void FGameplayTagPropertyMap::GameplayTagEventCallback(const FGameplayTag Tag, const int32 NewCount, const TWeakObjectPtr<> RegisteredOwner)
{
	// If the index and serial don't match with registered owner, the memory might be trashed so abort
	if (!ensure(RegisteredOwner.HasSameIndexAndSerialNumber(CachedOwner)))
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("FGameplayTagBlueprintPropertyMap::GameplayTagEventCallback called with corrupted Owner!"));
		return;
	}

	UObject* Owner = CachedOwner.Get();
	if (!Owner)
	{
		UE_LOG(LogAbilitySystem, Warning, TEXT("FGameplayTagBlueprintPropertyMap::GameplayTagEventCallback has an invalid Owner."));
		return;
	}

	// Find the property mapping corresponding to the given gameplay tag.
	const FGameplayTagBlueprintPropertyMapping* Mapping = PropertyMappings.FindByPredicate([Tag](const FGameplayTagBlueprintPropertyMapping& Test) { return Tag == Test.TagToMap; });

	// If found, update the bound property
	if (Mapping && Mapping->PropertyToEdit.Get())
	{
		if (const FBoolProperty* BoolProperty = CastField<const FBoolProperty>(Mapping->PropertyToEdit.Get()))
		{
			BoolProperty->SetPropertyValue_InContainer(Owner, NewCount > 0);
		}
		else if (const FIntProperty* IntProperty = CastField<const FIntProperty>(Mapping->PropertyToEdit.Get()))
		{
			IntProperty->SetPropertyValue_InContainer(Owner, NewCount);
		}
		else if (const FFloatProperty* FloatProperty = CastField<const FFloatProperty>(Mapping->PropertyToEdit.Get()))
		{
			FloatProperty->SetPropertyValue_InContainer(Owner, static_cast<float>(NewCount));
		}
	}
}

bool FGameplayTagPropertyMap::IsPropertyTypeValid(const FProperty* Property)
{
	check(Property);
	return Property->IsA<FBoolProperty>() || Property->IsA<FIntProperty>() || Property->IsA<FFloatProperty>();
}

EGameplayTagEventType::Type FGameplayTagPropertyMap::GetGameplayTagEventType(const FProperty* Property)
{
	check(Property);
	return Property->IsA(FBoolProperty::StaticClass()) ? EGameplayTagEventType::NewOrRemoved : EGameplayTagEventType::AnyCountChange;
}

EDataValidationResult FGameplayTagPropertyMap::IsDataValid(const UObject* OwningAsset, FDataValidationContext& Context) const
{
	const UClass* OwnerClass = OwningAsset != nullptr ? OwningAsset->GetClass() : nullptr;
	if (!OwnerClass)
	{
		UE_LOG(LogAbilitySystem, Error, TEXT("FGameplayTagBlueprintPropertyMap: IsDataValid() called with an invalid Owner."));
		return EDataValidationResult::Invalid;
	}

	for (const FGameplayTagBlueprintPropertyMapping& Mapping : PropertyMappings)
	{
		if (!Mapping.TagToMap.IsValid())
		{
			Context.AddError(
				FText::Format(FText::FromString("The gameplay tag [{0}] for property [{1}] is empty or invalid."), FText::AsCultureInvariant(Mapping.TagToMap.ToString()), FText::FromName(Mapping.PropertyName)));
		}

		if (const FProperty* Property = OwnerClass->FindPropertyByName(Mapping.PropertyName))
		{
			if (!IsPropertyTypeValid(Property))
			{
				Context.AddError(
					FText::Format(
						FText::FromString("The property [{0}] for gameplay tag [{1}] is not a supported type.  Supported types are: integer, float, and boolean."),
						FText::FromName(Mapping.PropertyName),
						FText::AsCultureInvariant(Mapping.TagToMap.ToString())));
			}
		}
		else
		{
			Context.AddError(
				FText::Format(FText::FromString("The property [{0}] for gameplay tag [{1}] could not be found."), FText::FromName(Mapping.PropertyName), FText::AsCultureInvariant(Mapping.TagToMap.ToString())));
		}
	}

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}