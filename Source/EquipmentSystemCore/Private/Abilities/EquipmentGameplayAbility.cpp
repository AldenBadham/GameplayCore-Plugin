// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/EquipmentGameplayAbility.h"
#include "Instances/EquipmentInstance.h"
#include "Instances/ItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif


UEquipmentGameplayAbility::UEquipmentGameplayAbility(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UEquipmentInstance* UEquipmentGameplayAbility::GetSourceEquipment() const
{
	if(FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<UEquipmentInstance>(Spec->SourceObject.Get());
	}
	return nullptr;
}

UItemInstance* UEquipmentGameplayAbility::GetSourceItem() const
{
	if (UEquipmentInstance* Equipment = GetSourceEquipment())
	{
		return Cast<UItemInstance>(Equipment->GetInstigator());
	}
	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UEquipmentGameplayAbility::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	// Ensure that equipment abilities are always instanced
	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(FText::FromString("Equipment abilities must be instanced."));
		Result = EDataValidationResult::Invalid;
	}
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

	return Result;
}
#endif
