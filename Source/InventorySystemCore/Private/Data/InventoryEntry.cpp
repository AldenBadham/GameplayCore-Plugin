// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/InventoryEntry.h"

#include "Components/InventorySystemComponent.h"
#include "Data/InventoryList.h"
#include "Instances/ItemInstance.h"
#include "Log/InventorySystemLog.h"

void FInventoryEntry::PreReplicatedRemove(const FInventoryList& InArraySerializer) const
{
	if (InArraySerializer.OwnerComponent)
	{
		UE_LOG(LogInventorySystem, Verbose, TEXT("%s: OnRemoveEntry (Non-Auth): %s. Count: %d"), *GetNameSafe(InArraySerializer.OwnerComponent->GetOwner()), *GetNameSafe(Instance), StackCount);
	}
}

void FInventoryEntry::PostReplicatedAdd(const FInventoryList& InArraySerializer) const
{
	if (InArraySerializer.OwnerComponent)
	{
		UE_LOG(LogInventorySystem, Verbose, TEXT("%s: OnAddedEntry (Non-Auth): %s. Count: %d"), *GetNameSafe(InArraySerializer.OwnerComponent->GetOwner()), *GetNameSafe(Instance), StackCount);
	}
}

FString FInventoryEntry::GetDebugString() const
{
	return FString::Printf(TEXT("(%s)"), *GetNameSafe(Instance));
}
