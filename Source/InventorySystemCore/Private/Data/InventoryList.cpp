// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/InventoryList.h"

#include "Components/InventorySystemComponent.h"
#include "Data/InventoryEntry.h"
#include "Definitions/Fragments/ItemFragment.h"
#include "Definitions/Fragments/ItemFragment_Storable.h"
#include "GameplayTags/InventoryGameplayTags.h"
#include "Instances/ItemInstance.h"
#include "Log/InventorySystemLog.h"

FInventoryList::FInventoryList()
{
}

FInventoryList::FInventoryList(UInventorySystemComponent* InOwnerComponent)
	: OwningComponent(InOwnerComponent)
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

FInventoryResult FInventoryList::AddFromDefinition(const TSubclassOf<UItemDefinition>& DefinitionClass, const int32 Count)
{
	FInventoryResult Result;

	if (!IsValid(DefinitionClass) || !OwningComponent)
	{
		UE_LOG(LogInventorySystem, Warning, TEXT("Tried to instantiate an item instance from invalid definition class."));
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidDefinition;
		return Result;
	}

	if (const AActor* OwnerActor = OwningComponent->GetOwner(); !OwnerActor->HasAuthority())
	{
		UE_LOG(LogInventorySystem, Warning, TEXT("Tried to instantiate an item instance from invalid definition class."));
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_NotAuthority;
		return Result;
	}

	// Check validity of the definition instance, and storable fragments
	if (!CanAdd(DefinitionClass, Result.FailureReason, Count))
	{
		return Result;
	}

	int32 RemainingCount = Count;

	const UItemDefinition* CachedDefinition = OwningComponent->GetCachedDefinition(DefinitionClass);
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

				Result.Instances.Add(Entry.Instance);
			}
		}
	}

	// Calculate the number of new stacks required
	const int32 MaxStackCount = StorableFragment->CanStack() ? StorableFragment->MaxStackCount : 1;
	const int32 NeededStacks = FMath::DivideAndRoundUp(RemainingCount, MaxStackCount);

	// Create new stacks for the rest
	for (int32 i = 0; i < NeededStacks && RemainingCount > 0; ++i)
	{
		if (!CanAdd(DefinitionClass, Result.FailureReason, RemainingCount))
		{
			break;
		}

		int32 CurrentCount = RemainingCount;
		if (UItemInstance* NewInstance = CreateItemInstance(DefinitionClass, CurrentCount))
		{
			Result.Instances.Add(NewInstance);
			RemainingCount = CurrentCount;
		}
	}

	return Result;
}

FInventoryResult FInventoryList::AddInstance(UItemInstance* ItemInstance, const int32 Count)
{
	FInventoryResult Result;

	if (!IsValid(ItemInstance))
	{
		Result.FailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidInstance;
		return Result;
	}

	// Check if the item can be added
	if (const TSubclassOf<UItemDefinition> DefinitionClass = ItemInstance->GetDefinitionClass())
	{
		if (!CanAdd(DefinitionClass, Result.FailureReason, Count))
		{
			return Result;
		}
	}

	// Creating and configuring the new input
	FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	const int32 NewIndex = Entries.Num() - 1;

	NewEntry.Instance = ItemInstance;
	NewEntry.StackCount = Count;
	NewEntry.LastStackCount = Count;

	Result.Instances.Add(ItemInstance);

	// Notification du changement
	Internal_OnEntryAdded(NewIndex, NewEntry);
	MarkItemDirty(NewEntry);

	return Result;
}

void FInventoryList::RemoveInstance(UItemInstance* Instance)
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

bool FInventoryList::RemoveFromHandle(const FInventoryEntryHandle& Handle, FGameplayTag& OutFailureReason)
{
	if (!Handle.IsHandleValid())
	{
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidHandle;
		return false;
	}

	if (!Entries.IsValidIndex(Handle.EntryIndex))
	{
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidHandle;
		return false;
	}

	const FInventoryEntry& Entry = Entries[Handle.EntryIndex];
	if (Entry.Instance != Handle.ItemInstance)
	{
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_HandleMismatch;
		return false;
	}

	Internal_OnEntryRemoved(Handle.EntryIndex, Entry);
	Entries.RemoveAt(Handle.EntryIndex);
	MarkArrayDirty();

	OutFailureReason = FGameplayTag::EmptyTag;
	return true;
}

bool FInventoryList::RemoveFromIndex(const int32 Index, FGameplayTag& OutFailureReason)
{
	if (!Entries.IsValidIndex(Index))
	{
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidIndex;
		return false;
	}

	Internal_OnEntryRemoved(Index, Entries[Index]);
	Entries.RemoveAt(Index);
	MarkArrayDirty();
	return true;
}

FInventoryEntryHandle FInventoryList::MakeHandle(const int32 Index) const
{
	if (!Entries.IsValidIndex(Index))
	{
		return FInventoryEntryHandle();
	}
	const FInventoryEntry& Entry = Entries[Index];
	return FInventoryEntryHandle(Index, Entry.Instance, Entry.StackCount, OwningContainer);
}

FInventoryEntryHandle FInventoryList::FindHandleFromInstance(UItemInstance* Instance) const
{
	FInventoryEntryHandle Handle;
	if (IsValid(Instance))
	{
		for (int32 Index = 0; Index < Entries.Num(); ++Index)
		{
			if (const FInventoryEntry& Entry = Entries[Index]; Entry.Instance == Instance)
			{
				Handle = FInventoryEntryHandle(Index, Entry);
				break;
			}
		}
	}
	return Handle;
}

FInventoryEntryHandle FInventoryList::FindHandleOfType(const TSubclassOf<UItemDefinition>& ItemDefinition)
{
	for (int Index = 0; Index < Entries.Num(); ++Index)
	{
		if (const FInventoryEntry& Entry = Entries[Index]; IsValid(Entry.Instance) && Entry.Instance->GetDefinitionClass() == ItemDefinition)
		{
			return FInventoryEntryHandle(Index, Entry);
		}
	}
	return {};
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

TArray<FInventoryEntry*> FInventoryList::GetAllEntries()
{
	TArray<FInventoryEntry*> InventoryEntries;

	// Pre-reserve memory for the array
	InventoryEntries.Reserve(Entries.Num());

	for (FInventoryEntry& Entry : Entries)
	{
		InventoryEntries.Add(&Entry);
	}
	return InventoryEntries;
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

	for (FInventoryEntry& Entry : Entries)
	{
		if (IsValid(Entry.Instance) && Entry.Instance->GetDefinitionClass()->IsInA(ItemDefinition))
		{
			InventoryEntries.Add(&Entry);
		}
	}
	return InventoryEntries;
}

int32 FInventoryList::GetStackCountByDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const
{
	int32 Count = 0;
	for (const FInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance && Entry.Instance->GetDefinitionClass() == ItemDefinitionClass)
		{
			++Count;
		}
	}
	return Count;
}

int32 FInventoryList::GetTotalCountByDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const
{
	int32 Total = 0;
	for (const FInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance && Entry.Instance->GetDefinitionClass() == ItemDefinitionClass)
		{
			Total += Entry.StackCount;
		}
	}
	return Total;
}

void FInventoryList::SetOwningComponent(UInventorySystemComponent* Component)
{
	OwningComponent = Component;
}

void FInventoryList::SetOwningContainer(UInventoryContainer* Container)
{
	OwningContainer = Container;
}

UItemInstance* FInventoryList::CreateItemInstance(const TSubclassOf<UItemDefinition>& DefinitionClass, int32& Count)
{
	AActor* OwnerActor = OwningComponent->GetOwner();
	if (!OwnerActor->HasAuthority())
	{
		return nullptr;
	}

	// Get item definition instance
	UItemDefinition* CachedDefinition = OwningComponent->GetCachedDefinition(DefinitionClass);
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
	Entry.OwningContainer = OwningContainer;
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

bool FInventoryList::CanAdd(const TSubclassOf<UItemDefinition>& DefinitionClass, FGameplayTag& OutFailureReason, const int32 InCount)
{
	UItemDefinition* CachedDefinition = OwningComponent->GetCachedDefinition(DefinitionClass);
	if (!IsValid(CachedDefinition))
	{
		UE_LOG(LogInventorySystem, Warning, TEXT("Tried to instantiate an item instance from invalid definition class."));
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidDefinition;
		return false;
	}

	if (!CachedDefinition->CanBeGiven(OwningComponent, OutFailureReason))
	{
		UE_LOG(LogInventorySystem, Warning, TEXT("Tried to instantiate an item instance from %s, but the definition conditions are not met."), *GetNameSafe(DefinitionClass));
		if (!OutFailureReason.IsValid())
		{
			OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_DefinitionRefused;
		}
		return false;
	}

	// If the object has no storable fragment, it cannot be added to the inventory
	const UItemFragment_Storable* StorableFragment = CachedDefinition->FindFragmentByClass<UItemFragment_Storable>();
	if (!IsValid(StorableFragment))
	{
		UE_LOG(LogInventorySystem, Warning, TEXT("Tried to instantiate an item instance of %s but do not own UItemFragment_Storable fragment."), *GetNameSafe(CachedDefinition));
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_NotStorable;
		return false;
	}

	// Check if the object is unique
	if (StorableFragment->IsUnique())
	{
		if (const FInventoryEntryHandle Handle = FindHandleOfType(DefinitionClass); Handle.IsHandleValid())
		{
			// Allow if we can fit in the existing stack
			if (StorableFragment->CanStack() && StorableFragment->MaxStackCount - Handle.StackCount <= InCount)
			{
				return true;
			}

			UE_LOG(LogInventorySystem, Warning, TEXT("Tried to instantiate a single item instance %s but another instance already exists."), *GetNameSafe(CachedDefinition));
			OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_Uniqueness;
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

	OwningComponent->PostInventoryEntryChanged(Data);
	OwningComponent->PostInventoryChanged(Data);
}

void FInventoryList::Internal_OnEntryAdded(const int32 Index, const FInventoryEntry& Entry) const
{
	FInventoryChangeData Data;
	Data.Index = Index;
	Data.Instance = Entry.Instance;
	Data.ChangeType = EInventoryChangeType::Added;
	Data.OldCount = Entry.LastStackCount;
	Data.NewCount = Entry.StackCount;

	OwningComponent->PostInventoryEntryAdded(Data);
	OwningComponent->PostInventoryChanged(Data);
}

void FInventoryList::Internal_OnEntryRemoved(const int32 Index, const FInventoryEntry& Entry) const
{
	FInventoryChangeData Data;
	Data.Index = Index;
	Data.Instance = Entry.Instance;
	Data.ChangeType = EInventoryChangeType::Removed;
	Data.OldCount = Entry.LastStackCount;
	Data.NewCount = Entry.StackCount;

	OwningComponent->PostInventoryEntryRemoved(Data);
	OwningComponent->PostInventoryChanged(Data);
}
