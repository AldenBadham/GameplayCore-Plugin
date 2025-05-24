// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

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
		if (Entries.IsValidIndex(Index))
		{
			FInventoryEntry& Entry = Entries[Index];
			Entry.LastStackCount = 0;

			Internal_OnEntryRemoved(Index, Entry);
		}
	}
}

void FInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		if (Entries.IsValidIndex(Index))
		{
			FInventoryEntry& Entry = Entries[Index];
			Entry.LastStackCount = Entry.StackCount;

			Internal_OnEntryAdded(Index, Entry);
		}
	}
}

void FInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		if (Entries.IsValidIndex(Index))
		{
			FInventoryEntry& Entry = Entries[Index];
			ensureMsgf(Entry.LastStackCount != INDEX_NONE, TEXT("LastStackCount is invalid (INDEX_NONE) for entry at index %d. Should replicate this change"), Index);

			Internal_OnEntryChanged(Index, Entry);
			Entry.LastStackCount = Entry.StackCount;
		}
	}
}

TArray<UItemInstance*> FInventoryList::Add(const TSubclassOf<UItemDefinition>& DefinitionClass, const int32 Count)
{
	if (!IsValid(DefinitionClass) || !OwnerComponent)
	{
		return {};
	}

	if (const AActor* OwnerActor = OwnerComponent->GetOwner(); !OwnerActor->HasAuthority())
	{
		return {};
	}

	// Check validity of the definition instance, and storable fragments
	if (!CanAdd(DefinitionClass, false))
	{
		return {};
	}

	TArray<UItemInstance*> Stacks = {};
	int32 RemainingCount = Count;

	const UItemDefinition* CachedDefinition = OwnerComponent->GetCachedDefinition(DefinitionClass);
	if (!IsValid(CachedDefinition))
	{
		return {};
	}

	const UItemFragment_Storable* StorableFragment = CachedDefinition->FindFragmentByClass<UItemFragment_Storable>();

	// Handles stacking if the object is stackable
	if (StorableFragment->CanStack())
	{
		for (int32 Index = 0; Index < Entries.Num() && RemainingCount > 0; ++Index)
		{
			FInventoryEntry& Entry = Entries[Index];
			if (Entry.Instance->DefinitionClass != DefinitionClass)
			{
				continue;
			}

			const int32 FreeCount = StorableFragment->MaxStackCount - Entry.StackCount;
			const int32 ToAdd = FMath::Min(RemainingCount, FreeCount);

			if (ToAdd > 0)
			{
				Entry.StackCount += ToAdd;
				RemainingCount -= ToAdd;

				Internal_OnEntryChanged(Index, Entry);
				Entry.LastStackCount = Entry.StackCount;
				MarkItemDirty(Entry);

				Stacks.Add(Entry.Instance);
			}
		}
	}

	// Calculate the number of new stacks required
	const int32 MaxStackCount = StorableFragment->CanStack() ? StorableFragment->MaxStackCount : 1;
	const int32 NeededStacks = FMath::DivideAndRoundUp(RemainingCount, MaxStackCount);

	// Create new stacks for the rest
	for (int32 i = 0; i < NeededStacks && RemainingCount > 0; ++i)
	{
		if (!CanAdd(DefinitionClass, true))
		{
			break;
		}

		int32 CurrentCount = RemainingCount;
		if (UItemInstance* NewInstance = CreateItemInstance(DefinitionClass, CurrentCount))
		{
			Stacks.Add(NewInstance);
			RemainingCount = CurrentCount;
		}
	}

	return Stacks;
}

void FInventoryList::Remove(UItemInstance* Instance)
{
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

void FInventoryList::AddItemInstance(UItemInstance* ItemInstance, const int32 Count)
{
	if (!IsValid(ItemInstance))
	{
		return;
	}

	// Check if the item can be added
	if (const TSubclassOf<UItemDefinition> DefinitionClass = ItemInstance->GetDefinitionClass(); !CanAdd(DefinitionClass, true))
	{
		return;
	}

	// Creating and configuring the new input
	FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	const int32 NewIndex = Entries.Num() - 1;

	NewEntry.Instance = ItemInstance;
	NewEntry.StackCount = Count;
	NewEntry.LastStackCount = Count;

	// Notification du changement
	Internal_OnEntryAdded(NewIndex, NewEntry);
	MarkItemDirty(NewEntry);
}

void FInventoryList::AddItemInstance(const FInventoryEntry& Entry)
{
	AddItemInstance(Entry.Instance, Entry.StackCount);
}

FInventoryEntryHandle FInventoryList::FindHandleOfType(const TSubclassOf<UItemDefinition>& ItemDefinition)
{
	if (!IsValid(ItemDefinition))
	{
		return {};
	}

	for (int Index = 0; Index < Entries.Num(); ++Index)
	{
		if (const FInventoryEntry& Entry = Entries[Index]; IsValid(Entry.Instance) && Entry.Instance->GetDefinitionClass() == ItemDefinition)
		{
			return FInventoryEntryHandle(Index, Entry);
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

	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		if (const FInventoryEntry& Entry = Entries[Index]; IsValid(Entry.Instance) && Entry.Instance->GetDefinitionClass()->IsInA(ItemDefinition))
		{
			FInventoryEntryHandle Handle = FInventoryEntryHandle(Index, Entry);
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

	for (int32 Index = 0; Index < Entries.Num(); ++Index)
	{
		if (const FInventoryEntry& Entry = Entries[Index]; IsValid(Entry.Instance))
		{
			FInventoryEntryHandle Handle = FInventoryEntryHandle(Index, Entry);
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

UItemInstance* FInventoryList::CreateItemInstance(const TSubclassOf<UItemDefinition>& DefinitionClass, int32& Count)
{
	AActor* OwnerActor = OwnerComponent->GetOwner();
	if (!OwnerActor->HasAuthority())
	{
		return nullptr;
	}

	// Get item definition instance
	UItemDefinition* CachedDefinition = OwnerComponent->GetCachedDefinition(DefinitionClass);
	const UItemFragment_Storable* StorableFragment = CachedDefinition->FindFragmentByClass<UItemFragment_Storable>();
	if (!IsValid(StorableFragment))
	{
		return nullptr;
	}

	// Create a new entry
	FInventoryEntry& Entry = Entries.AddDefaulted_GetRef();
	const int32 Index = Entries.Num();

	Entry.Instance = NewObject<UItemInstance>(OwnerActor);
	Entry.Instance->SetDefinition(CachedDefinition);

	Entry.StackCount = StorableFragment->CanStack() ? FMath::Min(Count, StorableFragment->MaxStackCount) : 1;
	Count -= Entry.StackCount;

	for (auto Fragment : CachedDefinition->Fragments)
	{
		if (IsValid(Fragment))
		{
			Fragment->OnInstanceCreated(Entry.Instance);
		}
	}

	Internal_OnEntryAdded(Index, Entry);
	MarkItemDirty(Entry);

	return Entry.Instance;
}

bool FInventoryList::CanAdd(const TSubclassOf<UItemDefinition>& DefinitionClass, const bool CheckUniqueness)
{
	UItemDefinition* CachedDefinition = OwnerComponent->GetCachedDefinition(DefinitionClass);
	if (!IsValid(CachedDefinition) || !CachedDefinition->CanBeGiven(OwnerComponent))
	{
		return false;
	}

	// If the object has no storable fragment, it cannot be added to the inventory
	const UItemFragment_Storable* StorableFragment = CachedDefinition->FindFragmentByClass<UItemFragment_Storable>();
	if (!IsValid(StorableFragment))
	{
		return false;
	}

	// Check if the object is unique
	if (CheckUniqueness && StorableFragment->IsUnique())
	{
		if (const FInventoryEntryHandle Handle = FindHandleOfType(DefinitionClass); Handle.IsValid())
		{
			return false;
		}
	}
	return true;
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
