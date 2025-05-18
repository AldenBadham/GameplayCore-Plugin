// Fill out your copyright notice in the Description page of Project Settings.


#include "Definitions/ItemDefinition.h"

#include "Definitions/Fragments/ItemFragment.h"

UItemDefinition::UItemDefinition(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.Get())
{
}

void UItemDefinition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UItemDefinition, Fragments))
	{
		TSet<UClass*> FragmentClasses;
		for (int32 i = Fragments.Num() - 1; i >= 0; --i)
		{
			if (const UItemFragment* Fragment = Fragments[i]; IsValid(Fragment) && FragmentClasses.Contains(Fragment->GetClass()))
			{
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

const UItemFragment* UItemDefinition::FindFragmentByClass(const TSubclassOf<UItemFragment> FragmentClass) const
{
	if (IsValid(FragmentClass))
	{
		for (const UItemFragment* Fragment : Fragments)
		{
			if (IsValid(Fragment) && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

bool UItemDefinition::HasFragmentByClass(const TSubclassOf<UItemFragment> FragmentClass) const
{
	if (IsValid(FragmentClass))
	{
		for (const UItemFragment* Fragment : Fragments)
		{
			return IsValid(Fragment) && Fragment->IsA(FragmentClass);
		}
	}

	return false;
}

bool UItemDefinition::CanGive(UInventorySystemComponent* InventorySystemComponent)
{
	return K2_CanGive(InventorySystemComponent);
}

bool UItemDefinition::K2_CanGive_Implementation(UInventorySystemComponent* InventorySystemComponent)
{
	return true;
}
