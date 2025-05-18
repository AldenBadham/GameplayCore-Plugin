#include "Attributes/AttributeSetBase.h"

#include "Components/AbilitySystemComponentBase.h"

UAttributeSetBase::UAttributeSetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
}

UAbilitySystemComponentBase* UAttributeSetBase::GetAbilitySystemComponentBase() const
{
	return Cast<UAbilitySystemComponentBase>(GetOwningAbilitySystemComponent());
}
