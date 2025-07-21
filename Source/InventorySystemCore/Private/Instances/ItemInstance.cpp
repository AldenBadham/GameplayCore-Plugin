// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Instances/ItemInstance.h"

#include "Components/InventorySystemComponent.h"
#include "Definitions/Fragments/ItemFragment.h"
#include "Interfaces/InventorySystemInterface.h"
#include "Net/UnrealNetwork.h"

UItemInstance::UItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OwningActor = GetTypedOuter<AActor>();
}

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DefinitionClass);
}

void UItemInstance::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (IsValid(Definition.Get()))
	{
		Definition->GetOwnedGameplayTags(TagContainer);
	}
	TagContainer.AppendTags(Tags);
}

void UItemInstance::Initialize(UItemDefinition* InDefinition)
{
	SetDefinition(InDefinition);

	for (UItemFragment* Fragment : InDefinition->Fragments)
	{
		if (IsValid(Fragment))
		{
			Fragment->OnInstanceCreated(this);
		}
	}

	PostInitialize();
}

void UItemInstance::Uninitialize()
{
	for (UItemComponent* Component : Components)
	{
		Component->Uninitialize();
	}
}

UInventorySystemComponent* UItemInstance::GetInventorySystemComponent() const
{
	if (!IsValid(OwningActor) || !OwningActor->Implements<UInventorySystemInterface>())
	{
		return nullptr;
	}

	if (const IInventorySystemInterface* Implementer = Cast<IInventorySystemInterface>(OwningActor))
	{
		return Implementer->GetInventorySystemComponent();
	}
	return nullptr;
}

AActor* UItemInstance::GetOwningActor() const
{
	return OwningActor;
}

TSubclassOf<UItemDefinition> UItemInstance::GetDefinitionClass() const
{
	return DefinitionClass;
}

UItemDefinition* UItemInstance::GetDefinition() const
{
	return Definition.Get();
}

const UItemFragment* UItemInstance::FindFragmentByClass(const TSubclassOf<UItemFragment> FragmentClass) const
{
	if (Definition.IsValid() && IsValid(FragmentClass))
	{
		return Definition->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

bool UItemInstance::HasFragmentByClass(const TSubclassOf<UItemFragment> FragmentClass) const
{
	if (Definition.IsValid() && IsValid(FragmentClass))
	{
		return Definition->HasFragmentByClass(FragmentClass);
	}

	return false;
}

UItemComponent* UItemInstance::AddComponent(const TSubclassOf<UItemComponent> ComponentClass)
{
	if (UItemComponent* Component = NewObject<UItemComponent>(this, ComponentClass); IsValid(Component))
	{
		Component->Initialize(*this);
		Components.Add(Component);
		return Component;
	}
	return nullptr;
}

void UItemInstance::PostInitialize()
{
	for (UItemComponent* Component : Components)
	{
		if (IsValid(Component))
		{
			Component->PostInitialize();	
		}
	}
}

void UItemInstance::SetDefinition(UItemDefinition* InDefinition)
{
	Definition = InDefinition;
	DefinitionClass = InDefinition->GetClass();
}

const UItemComponent* UItemInstance::FindComponentByClass(const TSubclassOf<UItemComponent> ComponentClass) const
{
	if (IsValid(ComponentClass))
	{
		for (const UItemComponent* Component : Components)
		{
			if (IsValid(Component) && Component->IsA(ComponentClass))
			{
				return Component;
			}
		}
	}

	return nullptr;
}
