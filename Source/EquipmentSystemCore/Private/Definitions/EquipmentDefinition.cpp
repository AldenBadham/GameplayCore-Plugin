// Fill out your copyright notice in the Description page of Project Settings.

#include "Definitions/EquipmentDefinition.h"

#include "Components/EquipmentSystemComponent.h"
#include "Definitions/Fragments/EquipmentFragment.h"
#include "Instances/EquipmentInstance.h"

UEquipmentDefinition::UEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
	InstanceClass = UEquipmentInstance::StaticClass();
}

#if WITH_EDITOR
void UEquipmentDefinition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Check if the Fragments property has changed
	if (PropertyChangedEvent.Property && PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UEquipmentDefinition, Fragments))
	{
		TSet<UClass*> FragmentClasses;
		for (int32 i = Fragments.Num() - 1; i >= 0; --i)
		{
			if (const UEquipmentFragment* Fragment = Fragments[i]; IsValid(Fragment) && FragmentClasses.Contains(Fragment->GetClass()))
			{
				// Remove duplicate fragments
				Fragments.RemoveAt(i);

				UE_LOG(LogTemp, Warning, TEXT("Only one fragment per class is allowed. Duplicate fragment removed."));
			}
			else if (Fragment)
			{
				FragmentClasses.Add(Fragment->GetClass());
			}
		}
	}
}
#endif

const UEquipmentFragment* UEquipmentDefinition::FindFragmentByClass(TSubclassOf<UEquipmentFragment> FragmentClass) const
{
	if (IsValid(FragmentClass))
	{
		for (const UEquipmentFragment* Fragment : Fragments)
		{
			if (IsValid(Fragment) && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

bool UEquipmentDefinition::CanEquip(UEquipmentSystemComponent* EquipmentSystemComponent)
{
	return K2_CanEquip(EquipmentSystemComponent);
}

bool UEquipmentDefinition::K2_CanEquip_Implementation(UEquipmentSystemComponent* EquipmentSystemComponent)
{
	return true;
}
