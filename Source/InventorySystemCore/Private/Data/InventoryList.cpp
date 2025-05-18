// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/InventoryList.h"

#include "Components/InventorySystemComponent.h"
#include "Data/InventoryEntry.h"
#include "Definitions/Fragments/ItemFragment.h"
#include "Definitions/Fragments/ItemFragment_Storable.h"
#include "Instances/ItemInstance.h"


FInventoryList::FInventoryList()
	: OwnerComponent(nullptr)
{
}

FInventoryList::FInventoryList(UInventorySystemComponent* InOwnerComponent)
	: OwnerComponent(InOwnerComponent)
{
}

void FInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		FInventoryEntry& Entry = Entries[Index];
		Entry.LastStackCount = 0;

		Internal_OnEntryRemoved(Index, Entry);
	}
}

void FInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		FInventoryEntry& Entry = Entries[Index];
		Entry.LastStackCount = Entry.StackCount;

		Internal_OnEntryAdded(Index, Entry);
	}
}

void FInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		FInventoryEntry& Entry = Entries[Index];
		check(Entry.LastStackCount != INDEX_NONE);
		Internal_OnEntryChanged(Index, Entry);
		Entry.LastStackCount = Entry.StackCount;
	}
}

UItemInstance* FInventoryList::Add(const TSubclassOf<UItemDefinition>& DefinitionClass, const int32 Count)
{
	if (!IsValid(DefinitionClass) || !OwnerComponent)
	{
		return nullptr;
	}

	AActor* OwnerActor = OwnerComponent->GetOwner();
	if (!OwnerActor->HasAuthority())
	{
		return nullptr;
	}

	UItemDefinition* CachedDefinition = OwnerComponent->GetItemDefinition(DefinitionClass);
	
	if(!CachedDefinition->CanGive(OwnerComponent))
		return nullptr;
	
	// If the object has no storable fragment, it cannot be added to the inventory
	const UItemFragment_Storable* StorableFragment = CachedDefinition->FindFragmentByClass<UItemFragment_Storable>();
	if (!IsValid(StorableFragment))
	{
		return nullptr;
	}

	// Check if the object is unique
	if (StorableFragment->IsUnique())
	{
		if (FindHandleOfType(DefinitionClass).IsValid())
		{
			return nullptr;
		}
	}

	// Handles stacking if the object is stackable
	if (StorableFragment->CanStack())
	{
		for (int Index = 0; Index < Entries.Num(); ++Index)
		{
			FInventoryEntry& Entry = Entries[Index];

			if(Entry.Instance->DefinitionClass != DefinitionClass)
				continue;

			const int32 FreeCount = StorableFragment->MaxStackCount - Entry.StackCount;
			if (const int32 ToAddCount = FMath::Min(Count, FreeCount); ToAddCount > 0)
			{
				Entry.StackCount += ToAddCount;
				Internal_OnEntryChanged(Index, Entry);
				Entry.LastStackCount = Entry.StackCount;

				MarkItemDirty(Entry);

				// If there is any item count to be added call it recursively
				if (ToAddCount < Count)
				{
					return Add(DefinitionClass, Count - ToAddCount);
				}

				return Entry.Instance;
			}
		}
	}

	// Create a new entry if remaining
	FInventoryEntry& Entry = Entries.AddDefaulted_GetRef();
	const int32 Index = Entries.Num();

	Entry.Instance = NewObject<UItemInstance>(OwnerActor);
	Entry.Instance->SetDefinition(CachedDefinition);
	
	Entry.StackCount = StorableFragment->CanStack() ? FMath::Min(Count, StorableFragment->MaxStackCount) : 1;

	for (auto Fragment : CachedDefinition->Fragments)
	{
		if (IsValid(Fragment))
		{
			Fragment->OnInstanceCreated(Entry.Instance);
		}
	}

	Internal_OnEntryAdded(Index, Entry);
	MarkItemDirty(Entry);

	// If there still remaining items to be added call it recursively
	if (Count > Entry.StackCount)
	{
		return Add(DefinitionClass, Count - Entry.StackCount);
	}

	return Entry.Instance;
}

void FInventoryList::Remove(UItemInstance* Instance)
{
	// TODO : To reinforce to cover tags and modifiers on items to not remove all items of the same type

	for (auto EntryIterator = Entries.CreateIterator(); EntryIterator; ++EntryIterator)
	{
		if (FInventoryEntry Entry = *EntryIterator; Entry.Instance == Instance)
		{
			Internal_OnEntryRemoved(EntryIterator.GetIndex(), Entry);
			EntryIterator.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

FInventoryEntryHandle FInventoryList::FindHandleOfType(const TSubclassOf<UItemDefinition>& ItemDefinition)
{
	if (!IsValid(ItemDefinition))
	{
		return {};
	}

	for (int i = 0; i < Entries.Num(); ++i)
	{
		if (const FInventoryEntry& Entry = Entries[i]; IsValid(Entry.Instance) && Entry.Instance->GetDefinitionClass()->IsInA(ItemDefinition))
		{
			FInventoryEntryHandle Handle;
			Handle.EntryIndex = i;
			Handle.ItemInstance = Entry.Instance;
			Handle.StackCount = Entry.StackCount;

			return Handle;
		}
	}
	return {};
}

TArray<FInventoryEntryHandle> FInventoryList::GetHandlesOfType(const TSubclassOf<UItemDefinition>& ItemDefinition)
{
	TArray<FInventoryEntryHandle> Handles = {};

	if (!IsValid(ItemDefinition))
	{
		return Handles;
	}

	for (int i = 0; i < Entries.Num(); ++i)
	{
		if (const FInventoryEntry& Entry = Entries[i]; IsValid(Entry.Instance) && Entry.Instance->GetDefinitionClass()->IsInA(ItemDefinition))
		{
			FInventoryEntryHandle Handle;
			Handle.EntryIndex = i;
			Handle.ItemInstance = Entry.Instance;
			Handle.StackCount = Entry.StackCount;

			Handles.Add(Handle);
		}
	}
	return Handles;
}

TArray<FInventoryEntryHandle> FInventoryList::GetAllHandles() const
{
	TArray<FInventoryEntryHandle> Handles = {};

	// Pre-reserve memory for the array
	Handles.Reserve(Entries.Num());

	for (int i = 0; i < Entries.Num(); ++i)
	{
		if (const FInventoryEntry& Entry = Entries[i]; IsValid(Entry.Instance))
		{
			FInventoryEntryHandle Handle;
			Handle.EntryIndex = i;
			Handle.ItemInstance = Entry.Instance;
			Handle.StackCount = Entry.StackCount;

			Handles.Add(Handle);
		}
	}
	return Handles;
}

FInventoryEntry* FInventoryList::FindEntryOfType(const TSubclassOf<UItemDefinition>& ItemDefinition)
{
	if (!IsValid(ItemDefinition))
	{
		return nullptr;
	}

	return Entries.FindByPredicate([ItemDefinition](const FInventoryEntry& Entry) { return Entry.Instance->GetDefinitionClass()->IsInA(ItemDefinition); });
}

TArray<FInventoryEntry*> FInventoryList::GetEntriesOfType(const TSubclassOf<UItemDefinition>& ItemDefinition)
{
	TArray<FInventoryEntry*> InventoryEntries = {};

	if (!IsValid(ItemDefinition))
	{
		return InventoryEntries;
	}

	for (auto Entry : InventoryEntries)
	{
		if (IsValid(Entry->Instance) && Entry->Instance->GetDefinitionClass()->IsInA(ItemDefinition))
		{
			InventoryEntries.Add(Entry);
		}
	}
	return InventoryEntries;
}

TArray<FInventoryEntry*> FInventoryList::GetAllEntries()
{
	TArray<FInventoryEntry*> InventoryEntries;

	// Pre-reserve memory for the array
	InventoryEntries.Reserve(InventoryEntries.Num());

	for (auto Entry : InventoryEntries)
	{
		InventoryEntries.Add(Entry);
	}
	return InventoryEntries;
}

void FInventoryList::Internal_OnEntryChanged(const int32 Index, const FInventoryEntry& Entry) const
{
	FInventoryChangeData Data;
	Data.Index = Index;
	Data.Instance = Entry.Instance;
	Data.ChangeType = EInventoryChangeType::Modified;
	Data.OldCount = Entry.LastStackCount;
	Data.NewCount = Entry.StackCount;

	OwnerComponent->PostInventoryEntryChanged(Data);
	OwnerComponent->PostInventoryChanged(Data);
}

void FInventoryList::Internal_OnEntryAdded(const int32 Index, const FInventoryEntry& Entry) const
{
	FInventoryChangeData Data;
	Data.Index = Index;
	Data.Instance = Entry.Instance;
	Data.ChangeType = EInventoryChangeType::Added;
	Data.OldCount = Entry.LastStackCount;
	Data.NewCount = Entry.StackCount;

	OwnerComponent->PostInventoryEntryAdded(Data);
	OwnerComponent->PostInventoryChanged(Data);
}

void FInventoryList::Internal_OnEntryRemoved(const int32 Index, const FInventoryEntry& Entry) const
{
	FInventoryChangeData Data;
	Data.Index = Index;
	Data.Instance = Entry.Instance;
	Data.ChangeType = EInventoryChangeType::Removed;
	Data.OldCount = Entry.LastStackCount;
	Data.NewCount = Entry.StackCount;

	OwnerComponent->PostInventoryEntryRemoved(Data);
	OwnerComponent->PostInventoryChanged(Data);
}
