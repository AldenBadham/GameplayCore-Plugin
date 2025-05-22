#include "Instances/ItemInstance.h"

#include "Definitions/Fragments/ItemFragment.h"
#include "Net/UnrealNetwork.h"

UItemInstance::UItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OwningController = GetTypedOuter<APlayerController>();
}

void UItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DefinitionClass);
}

UInventorySystemComponent* UItemInstance::GetInventorySystemComponent() const
{
	if (!IsValid(OwningController))
	{
		return nullptr;
	}

	if (UInventorySystemComponent* InventorySystemComp = OwningController->GetComponentByClass<UInventorySystemComponent>(); IsValid(InventorySystemComp))
	{
		return InventorySystemComp;
	}
	return nullptr;
}

APlayerController* UItemInstance::GetOwningController() const
{
	return OwningController;
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
		Components.Add(Component);
		return Component;
	}
	return nullptr;
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