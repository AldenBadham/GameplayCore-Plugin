// Licensed under the MIT License. See the LICENSE file in the project root for full license information.


#include "Containers/InventoryContainer.h"

#include "Containers/Policies/StoragePolicy.h"
#include "Definitions/ItemDefinition.h"
#include "Engine/ActorChannel.h"
#include "GameplayTags/InventoryGameplayTags.h"
#include "Instances/ItemInstance.h"
#include "Net/UnrealNetwork.h"

UInventoryContainer::UInventoryContainer(const FObjectInitializer& ObjectInitializer)
{
	InventoryList.SetOwningContainer(this);
}

void UInventoryContainer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryContainer, InventoryList);
}

bool UInventoryContainer::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bReplicated = true;
	for (FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (UItemInstance* Instance = Entry.Instance; IsValid(Instance))
		{
			bReplicated |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}
	return bReplicated;
}

FInventoryResult UInventoryContainer::TryAddItemDefinition(const TSubclassOf<UItemDefinition> Definition, const int32 Count)
{
	FInventoryResult Result;
	if (!IsValid(Definition))
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidDefinition;
		return Result;
	}
	if (Count <= 0)
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidCount;
		return Result;
	}

	Result = InventoryList.AddFromDefinition(Definition, Count);
	return Result;
}

FInventoryResult UInventoryContainer::TryAddItemInstance(UItemInstance* Instance, const int32 Count)
{
	FInventoryResult Result;
	if (!IsValid(Instance))
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidInstance;
		return Result;
	}
	if (Count <= 0)
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidCount;
		return Result;
	}
	if (!ValidateStorage(Instance, Result.FailureReason))
	{
		return Result;
	}

	Result = InventoryList.AddInstance(Instance, Count);
	return Result;
}

bool UInventoryContainer::TryRemoveItem(FInventoryEntryHandle& Handle, FGameplayTag& OutFailureReason)
{
	if (!Handle.IsHandleValid())
	{
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidInstance;
		return false;
	}

	if (InventoryList.RemoveFromHandle(Handle, OutFailureReason))
	{
		return true;
	}
	OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidHandle;
	return false;
}

FInventoryResult UInventoryContainer::TryMoveItemTo(FInventoryEntryHandle Handle, UInventoryContainer* TargetContainer)
{
	FInventoryResult Result;

	if (!Handle.IsHandleValid())
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidHandle;
		return Result;
	}
	if (!IsValid(TargetContainer))
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidContainer;
		return Result;
	}

	UItemInstance* Instance = Handle.ItemInstance;
	if (!IsValid(Instance))
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidInstance;
		return Result;
	}

	Result = TargetContainer->TryAddItemInstance(Instance, Handle.StackCount);
	if (!Result.Succeeded())
	{
		return Result;
	}

	if (!TryRemoveItem(Handle, Result.FailureReason))
	{
		return Result;
	}

	return Result;
}

FInventoryEntryHandle UInventoryContainer::FindHandle(UItemInstance* Instance) const
{
	return InventoryList.FindHandleFromInstance(Instance);
}

void UInventoryContainer::SetOwnerComponent(UInventorySystemComponent* NewOwner)
{
	OwnerComponent = NewOwner;
	InventoryList.SetOwningComponent(NewOwner);
}

int32 UInventoryContainer::GetStackCountByDefinition(const TSubclassOf<UItemDefinition>& DefinitionClass) const
{
	return InventoryList.GetStackCountByDefinition(DefinitionClass);
}

int32 UInventoryContainer::GetTotalCountByDefinition(const TSubclassOf<UItemDefinition>& DefinitionClass) const
{
	return InventoryList.GetTotalCountByDefinition(DefinitionClass);
}

bool UInventoryContainer::ValidateStorage(UItemInstance* Instance, FGameplayTag& OutFailureReason) const
{
	for (const TObjectPtr<UStoragePolicy>& Policy : Policies)
	{
		if (IsValid(Policy) && !Policy->CanStoreItem(Instance, OutFailureReason))
		{
			return false;
		}
	}

	OutFailureReason = FGameplayTag::EmptyTag;
	return true;
}

void UInventoryContainer::AddStoragePolicy(UStoragePolicy* Policy)
{
	if (IsValid(Policy))
	{
		Policies.Add(Policy);
	}
}
