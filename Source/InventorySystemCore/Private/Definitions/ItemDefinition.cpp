// Fill out your copyright notice in the Description page of Project Settings.

#include "Definitions/ItemDefinition.h"

#include "Definitions/Fragments/ItemFragment.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Log/InventorySystemLog.h"
#include "Settings/InventorySystemSettings.h"
#include "Settings/ItemFragmentRule.h"
#include "Widgets/Notifications/SNotificationList.h"

UItemDefinition::UItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
}
void UItemDefinition::PostLoad()
{
	UObject::PostLoad();

#if WITH_EDITORONLY_DATA
	PreviousFragments = Fragments;
#endif
}

#if WITH_EDITOR
void UItemDefinition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UItemDefinition, Fragments))
	{
		// Check Fragment rules from the Inventory System Settings
		const UInventorySystemSettings* InventorySettings = GetDefault<UInventorySystemSettings>();
		UE_CLOG(IsValid(InventorySettings), LogInventorySystem, Error, TEXT("Invalid Inventory System Settings class get."));

		// Identify newly added fragments
		TMap<int32, UItemFragment*> NewFragments;
		TArray<TSubclassOf<UItemFragment>> ExistingClasses;

		for (int32 Index = 0; Index < Fragments.Num(); ++Index)
		{
			if (UItemFragment* const Fragment = Fragments[Index]; IsValid(Fragment))
			{
				if (!PreviousFragments.Contains(Fragment))
				{
					NewFragments.Add(Index, Fragment);
					continue;
				}
				ExistingClasses.Add(Fragment->GetClass());
			}
		}

		// Quick duplicate check
		for (auto& [Index, NewFragment] : NewFragments)
		{
			TSubclassOf<UItemFragment> NewFragmentClass = NewFragment->GetClass();
			if (ExistingClasses.Contains(NewFragmentClass))
			{
				FString Context = FString::Printf(TEXT("Duplicated item fragment on %s."), *this->GetName());

				FNotificationInfo Info(FText::FromString(Context));
				FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(SNotificationItem::CS_Fail);

				UE_LOG(LogInventorySystem, Error, TEXT("Duplicated item fragment %s of %s."), *NewFragmentClass->GetName(), *this->GetName());

				if (Fragments.IsValidIndex(Index))
				{
					Fragments[Index] = nullptr;
				}
			}
		}

		// Check rule validity for new fragments
		for (auto& [Index, NewFragment] : NewFragments)
		{
			TSubclassOf<UItemFragment> NewFragmentClass = NewFragment->GetClass();
			if (const FItemFragmentRule* Rule = InventorySettings->FindRuleForClass(NewFragmentClass))
			{
				if (FString ErrorMessage; !Rule->IsRuleSatisfied(ExistingClasses, ErrorMessage))
				{
					FString Context = FString::Printf(TEXT("Fragment rule not filled on %s."), *this->GetName());

					FNotificationInfo Info(FText::FromString(Context));
					FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(SNotificationItem::CS_Fail);

					UE_LOG(LogInventorySystem, Error, TEXT("Fragment rule not filled for fragment %s of %s. %s"), *NewFragmentClass->GetName(), *this->GetName(), *ErrorMessage);

					if (Fragments.IsValidIndex(Index))
					{
						Fragments[Index] = nullptr;
					}
				}
			}
		}

		// Previous fragments update
		PreviousFragments = Fragments;

		PostEditChange();
	}
}
#endif

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
	const bool bFound = FindFragmentByClass(FragmentClass) != nullptr;
	return bFound;
}

bool UItemDefinition::CanBeGiven(UInventorySystemComponent* InventorySystemComponent)
{
	return K2_CanBeGiven(InventorySystemComponent);
}

bool UItemDefinition::K2_CanBeGiven_Implementation(UInventorySystemComponent* InventorySystemComponent)
{
	return true;
}
