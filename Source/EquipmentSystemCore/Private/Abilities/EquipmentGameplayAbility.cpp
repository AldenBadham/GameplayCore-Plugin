#include "Abilities/EquipmentGameplayAbility.h"
#include "Instances/EquipmentInstance.h"
#include "Instances/ItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UEquipmentGameplayAbility::UEquipmentGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UEquipmentInstance* UEquipmentGameplayAbility::GetSourceEquipmentInstance() const
{
	return TryGetSourceObject<UEquipmentInstance>();
}

UEquipmentInstance* UEquipmentGameplayAbility::GetTypedSourceEquipmentInstance(const TSubclassOf<UEquipmentInstance>& InstanceClass) const
{
	if (UEquipmentInstance* Instance = TryGetSourceObject<UEquipmentInstance>(); IsValid(Instance) && Instance->IsA(InstanceClass))
	{
		return Instance;
	}
	return nullptr;
}

UItemInstance* UEquipmentGameplayAbility::GetSourceItemInstance() const
{
	if (const UEquipmentInstance* Instance = GetSourceEquipmentInstance(); IsValid(Instance))
	{
		return Cast<UItemInstance>(Instance->GetSourceItem());
	}
	return nullptr;
}

UItemInstance* UEquipmentGameplayAbility::GetTypedSourceItemInstance(const TSubclassOf<UItemInstance>& InstanceClass) const
{
	if (UItemInstance* Instance = GetSourceItemInstance(); IsValid(Instance) && Instance->IsA(InstanceClass))
	{
		return Instance;
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