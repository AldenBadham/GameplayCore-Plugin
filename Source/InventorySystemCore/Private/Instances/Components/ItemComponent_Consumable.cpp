#include "Instances/Components/ItemComponent_Consumable.h"

#include "Net/UnrealNetwork.h"

void UItemComponent_Consumable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentUse);
}