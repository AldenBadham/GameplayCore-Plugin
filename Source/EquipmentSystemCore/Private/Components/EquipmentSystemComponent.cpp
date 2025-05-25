// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/EquipmentSystemComponent.h"

#include "Data/EquipmentCache.h"
#include "Data/EquipmentEntry.h"
#include "Data/Slots/EquipmentSlotMapData.h"
#include "Data/Slots/SlotDefinition.h"
#include "Definitions/Fragments/ItemFragment_Equippable.h"
#include "Engine/ActorChannel.h"
#include "GameplayTags/EquipmentGameplayTags.h"
#include "Instances/EquipmentInstance.h"
#include "Instances/ItemInstance.h"
#include "Log/EquipmentSystemLog.h"
#include "Net/UnrealNetwork.h"
#include "Policies/SlotPolicy.h"

#include "Stats/EquipmentSystemStats.h"

#define LOCTEXT_NAMESPACE "UEquipmentSystemComponent"

UEquipmentSystemComponent::UEquipmentSystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), EquipmentList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UEquipmentSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

bool UEquipmentSystemComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bReplicated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance; IsValid(Instance))
		{
			bReplicated |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bReplicated;
}

void UEquipmentSystemComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register all equipment instances ready for replication with the actor component
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FEquipmentEntry& Entry : EquipmentList.Entries)
		{
			if (UEquipmentInstance* Instance = Entry.Instance; IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

void UEquipmentSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache initialization
	Cache = NewObject<UEquipmentCache>(this, "Cache");

	if (IsValid(SlotMapData))
	{
		for (const FSlotDefinition& Def : SlotMapData->Slots)
		{
			if (Def.SlotTag.IsValid())
			{
				SlotMapRuntime.Add(Def.SlotTag, FDynamicEquipmentSlot{});
			}
		}
	}
}

void UEquipmentSystemComponent::UninitializeComponent()
{
	TArray<FGameplayTag> SlotsToUnequip;
	for (const auto& Pair : SlotMapRuntime)
	{
		if (IsValid(Pair.Value.ItemInstance))
		{
			SlotsToUnequip.Add(Pair.Key);
		}
	}

	FGameplayTag FailureReason;
	for (const FGameplayTag& SlotTag : SlotsToUnequip)
	{
		TryUnequipSlot(SlotTag, FailureReason);
	}

	Super::UninitializeComponent();
}

FEquipmentResult UEquipmentSystemComponent::TryEquipItem(UItemInstance* ItemInstance)
{
	FEquipmentResult Result;
	if (!IsValid(ItemInstance))
	{
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_NullItem;
		return Result;
	}

	const UItemFragment_Equippable* Frag = ItemInstance->FindFragmentByClass<UItemFragment_Equippable>();
	if (!Frag || !Frag->EquipmentDefinition)
	{
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_MissingDefinition;
		return Result;
	}

	const UEquipmentDefinition* Definition = GetCachedDefinition(Frag->EquipmentDefinition);
	const FGameplayTag SlotTag = Definition->SlotTag;

	return TryEquipItemOnSlot(ItemInstance, Definition->SlotTag);
}

FEquipmentResult UEquipmentSystemComponent::TryEquipItemOnSlot(UItemInstance* ItemInstance, const FGameplayTag& SlotTag)
{
	FEquipmentResult Result;
	if (!IsValid(ItemInstance))
	{
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_NullItem;
		return Result;
	}

	const UItemFragment_Equippable* Frag = ItemInstance->FindFragmentByClass<UItemFragment_Equippable>();
	if (!Frag || !Frag->EquipmentDefinition)
	{
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_MissingDefinition;
		return Result;
	}

	const UEquipmentDefinition* Definition = GetCachedDefinition(Frag->EquipmentDefinition);

	// De-equip the current slot if necessary
	if (const FDynamicEquipmentSlot* CurrentSlot = FindSlot(SlotTag))
	{
		if (IsValid(CurrentSlot->ItemInstance))
		{
			if (!Internal_ProcessUnequip(SlotTag, Result.FailureReason))
			{
				return Result;
			}
		}
	}

	return Internal_ProcessEquip(ItemInstance, SlotTag, Definition);
}

bool UEquipmentSystemComponent::TryUnequipItem(UItemInstance* ItemInstance, FGameplayTag& OutFailureReason)
{
	if (!IsValid(ItemInstance))
	{
		OutFailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_NullItem;
		return false;
	}

	const FGameplayTag Slot = GetSlotForItem(ItemInstance);
	if (!Slot.IsValid())
	{
		OutFailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_SlotNotFound;
		return false;
	}

	return TryUnequipSlot(Slot, OutFailureReason);
}

bool UEquipmentSystemComponent::TryUnequipSlot(const FGameplayTag SlotTag, FGameplayTag& OutFailureReason)
{
	return Internal_ProcessUnequip(SlotTag, OutFailureReason);
}

bool UEquipmentSystemComponent::TrySwapSlots(const FGameplayTag& SlotA, const FGameplayTag& SlotB, FGameplayTag& OutFailureReason)
{
	// First validate that both slots exist in our runtime map
	if (!SlotMapRuntime.Contains(SlotA) || !SlotMapRuntime.Contains(SlotB))
	{
		OutFailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_SlotNotFound;
		return false;
	}

	const FDynamicEquipmentSlot& SlotDataA = SlotMapRuntime.FindChecked(SlotA);
	const FDynamicEquipmentSlot& SlotDataB = SlotMapRuntime.FindChecked(SlotB);

	// If both slots are empty, no swap is needed
	if (!IsValid(SlotDataA.ItemInstance) && !IsValid(SlotDataB.ItemInstance))
	{
		return true;
	}

	// Store slot A contents before we start moving things around
	UItemInstance* TempItemA = SlotDataA.ItemInstance;
	UEquipmentInstance* TempEquipA = SlotDataA.EquipmentInstance;

	// First phase: Unequip both slots while preserving the item references
	if (IsValid(SlotDataA.ItemInstance))
	{
		if (!Internal_ProcessUnequip(SlotA, OutFailureReason, true))
		{
			return false;
		}
	}

	if (IsValid(SlotDataB.ItemInstance))
	{
		if (!Internal_ProcessUnequip(SlotB, OutFailureReason, true))
		{
			// On failure, restore slot A to its original state
			if (IsValid(TempItemA))
			{
				if (const UItemFragment_Equippable* Frag = TempItemA->FindFragmentByClass<UItemFragment_Equippable>(); Frag->EquipmentDefinition)
				{
					Internal_ProcessEquip(TempItemA, SlotA, GetCachedDefinition(Frag->EquipmentDefinition));
				}
			}
			return false;
		}
	}

	// Second phase: Cross-equip items into their new slots
	bool bSuccess = true;
	if (IsValid(SlotDataB.ItemInstance))
	{
		if (const UItemFragment_Equippable* Frag = SlotDataB.ItemInstance->FindFragmentByClass<UItemFragment_Equippable>(); Frag->EquipmentDefinition)
		{
			const FEquipmentResult Result = Internal_ProcessEquip(SlotDataB.ItemInstance, SlotA, GetCachedDefinition(Frag->EquipmentDefinition));
			bSuccess = Result.Succeeded();
		}
	}

	if (bSuccess && IsValid(TempItemA))
	{
		if (const UItemFragment_Equippable* Frag = TempItemA->FindFragmentByClass<UItemFragment_Equippable>(); Frag->EquipmentDefinition)
		{
			const FEquipmentResult Result = Internal_ProcessEquip(TempItemA, SlotB, GetCachedDefinition(Frag->EquipmentDefinition));
			bSuccess = Result.Succeeded();
		}
	}

	return bSuccess;
}

bool UEquipmentSystemComponent::IsSlotBlocked(const FGameplayTag& SlotTag, const bool bExact) const
{
	if (bExact)
	{
		return BlockedSlots.GetTagCount(SlotTag) > 0;
	}
	return BlockedSlots.HasMatchingGameplayTag(SlotTag);
}

const FDynamicEquipmentSlot* UEquipmentSystemComponent::FindSlot(const FGameplayTag& SlotTag) const
{
	return SlotMapRuntime.Find(SlotTag);
}

const FGameplayTag& UEquipmentSystemComponent::GetSlotForItem(UItemInstance* ItemInstance)
{
	if (IsValid(ItemInstance))
	{
		for (const auto& [SlotTag, SlotData] : SlotMapRuntime)
		{
			if (SlotData.ItemInstance == ItemInstance)
			{
				return SlotTag;
			}
		}
	}
	return FGameplayTag::EmptyTag;
}

UItemInstance* UEquipmentSystemComponent::GetItemInSlot(const FGameplayTag& SlotTag)
{
	if (SlotMapRuntime.Contains(SlotTag))
	{
		return SlotMapRuntime.FindChecked(SlotTag).ItemInstance;
	}
	return nullptr;
}

UEquipmentInstance* UEquipmentSystemComponent::GetEquipmentInSlot(const FGameplayTag& SlotTag)
{
	if (SlotMapRuntime.Contains(SlotTag))
	{
		return SlotMapRuntime.FindChecked(SlotTag).EquipmentInstance;
	}
	return nullptr;
}


void UEquipmentSystemComponent::GetEquippedItems(TMap<FGameplayTag, UItemInstance*>& OutItems) const
{
	OutItems.Empty();
	for (const auto& [SlotTag, SlotData] : SlotMapRuntime)
	{
		if (SlotData.ItemInstance)
		{
			OutItems.Add(SlotTag, SlotData.ItemInstance);
		}
	}
}

UEquipmentInstance* UEquipmentSystemComponent::GetInstanceOfType(const TSubclassOf<UEquipmentInstance>& InstanceClass)
{
	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceClass))
			{
				return Instance;
			}
		}
	}
	return nullptr;
}

TArray<UEquipmentInstance*> UEquipmentSystemComponent::GetAllInstancesOfType(const TSubclassOf<UEquipmentInstance>& InstanceClass)
{
	TArray<UEquipmentInstance*> Results;
	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceClass))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}

UEquipmentDefinition* UEquipmentSystemComponent::GetCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class) const
{
	if (IsValid(Cache))
	{
		return Cache->GetCachedDefinition(Class);
	}
	if (IsValid(Class))
	{
		return NewObject<UEquipmentDefinition>(GetOuter(), Class);
	}
	return nullptr;
}

UEquipmentInstance* UEquipmentSystemComponent::GetInstanceFromItem(UItemInstance* ItemInstance)
{
	for (FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->GetSourceItem() == ItemInstance)
			{
				return Instance;
			}
		}
	}
	return nullptr;
}

void UEquipmentSystemComponent::PostEquipmentEquipped(const FEquipmentChangeData& Data)
{
	OnEquipmentEquipped.Broadcast(Data);
}

void UEquipmentSystemComponent::PostEquipmentUnequipped(const FEquipmentChangeData& Data)
{
	OnEquipmentUnequipped.Broadcast(Data);
}

void UEquipmentSystemComponent::PostEquipmentChanged(const FEquipmentChangeData& Data)
{
	OnEquipmentChanged.Broadcast(Data);
}

void UEquipmentSystemComponent::LockSlots(const FGameplayTagContainer& Tags)
{
	BlockedSlots.UpdateTagCount(Tags, 1);
}

void UEquipmentSystemComponent::UnlockSlots(const FGameplayTagContainer& Tags)
{
	BlockedSlots.UpdateTagCount(Tags, -1);
}

FEquipmentResult UEquipmentSystemComponent::Internal_ProcessEquip(UItemInstance* ItemInstance, const FGameplayTag& TargetSlot, const UEquipmentDefinition* Definition)
{
	SCOPE_CYCLE_COUNTER(STAT_Equipment_ProcessEquip);

	FEquipmentResult Result;
	// Validate input parameters
	if (!IsValid(ItemInstance))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process equipment of a null item instance."));
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_NullItem;
		return Result;
	}

	if (!IsValid(Definition))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process equipment of %s with a null equipment definition."), *GetNameSafe(ItemInstance));
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_MissingDefinition;
		return Result;
	}

	// Validate slot exists in our equipment system
	if (!FindSlot(TargetSlot))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process equipment of %s as %s but did not found slot %s in the slot map."), *GetNameSafe(ItemInstance), *GetNameSafe(Definition), *TargetSlot.ToString());
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_SlotNotFound;
		return Result;
	}

	// Check if slot is currently blocked by another item
	if (IsSlotBlocked(TargetSlot, true))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process equipment of %s as %s on slot %s but the slot is blocked."), *GetNameSafe(ItemInstance), *GetNameSafe(Definition), *TargetSlot.ToString());
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_SlotBlocked;
		return Result;
	}

	// Check if the slot policy allows this equipment
	if (const USlotPolicy* Policy = Definition->SlotPolicy)
	{
		if (!Policy->CanEquipItem(ItemInstance, TargetSlot, this))
		{
			Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_PolicyRefused;
			return Result;
		}
	}

	// Perform the actual equipment operation
	Result = Internal_EquipOnSlot(TargetSlot, ItemInstance, Definition);
	if (!Result.Succeeded())
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process equipment of %s as %s on slot %s but failed to instantiate the equipment."), *GetNameSafe(ItemInstance), *GetNameSafe(Definition), *TargetSlot.ToString());
		Result.FailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_Internal;
		return Result;
	}

	return Result;
}

bool UEquipmentSystemComponent::Internal_ProcessUnequip(const FGameplayTag& SlotTag, FGameplayTag& OutFailureReason, const bool bPreserveItem)
{
	SCOPE_CYCLE_COUNTER(STAT_Equipment_ProcessUnequip);

	// Validate that the slot exists in our runtime map
	if (!SlotMapRuntime.Contains(SlotTag))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process un-equipment but did not found slot %s in the slot map."), *SlotTag.ToString());
		OutFailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_SlotNotFound;
		return false;
	}

	auto& SlotData = SlotMapRuntime.FindChecked(SlotTag);

	// Check if there's actually something equipped in this slot
	if (!IsValid(SlotData.ItemInstance) || !IsValid(SlotData.EquipmentInstance))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process un-equipment but the slot %s is empty."), *SlotTag.ToString());
		OutFailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_SlotEmpty;
		return false;
	}

	// Validate that the item has the required equippable fragment
	const UItemFragment_Equippable* Frag = SlotData.ItemInstance->FindFragmentByClass<UItemFragment_Equippable>();
	if (!Frag || !Frag->EquipmentDefinition)
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process un-equipment on the slot %s but did found a non-equipable item."), *SlotTag.ToString());
		OutFailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_MissingDefinition;
		return false;
	}

	// Get and validate the equipment definition
	const UEquipmentDefinition* Def = GetCachedDefinition(Frag->EquipmentDefinition);
	if (!IsValid(Def))
	{
		UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to process un-equipment on the slot %s but impossible to get the equipment definition."), *SlotTag.ToString());
		OutFailureReason = EquipmentSystemGameplayTags::TAG_Equipment_Failure_Internal;
		return false;
	}

	// Remove any slot blocking applied by this equipment's policy
	if (const USlotPolicy* Policy = Def->SlotPolicy)
	{
		UnlockSlots(Policy->GetBlockedSlotTags());
	}

	// Store the instance before clearing slot data
	UEquipmentInstance* Instance = SlotData.EquipmentInstance;
	if (!bPreserveItem)
	{
		SlotData.ItemInstance = nullptr;
	}
	SlotData.EquipmentInstance = nullptr;

	// Clean up the equipment instance
	if (IsValid(Instance))
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(Instance);
		}
		Instance->OnUnequipped();
		EquipmentList.Remove(Instance, OutFailureReason);
	}

	OutFailureReason = FGameplayTag::EmptyTag;
	return true;
}


FEquipmentResult UEquipmentSystemComponent::Internal_EquipOnSlot(const FGameplayTag& SlotTag, UItemInstance* ItemInstance, const UEquipmentDefinition* Definition)
{
	SCOPE_CYCLE_COUNTER(STAT_Equipment_EquipOnSlot);

	// Create a new equipment instance and add it to our list
	FEquipmentResult Result = EquipmentList.Add(Definition->GetClass(), ItemInstance);
	if (!Result.Succeeded())
	{
		return Result;
	}

	// Notify the instance it has been equipped
	Result.Instance->OnEquipped();

	// Setup replication if needed
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(Result.Instance);
	}

	// Update the runtime slot with the new equipment
	FDynamicEquipmentSlot& RuntimeSlot = SlotMapRuntime.FindChecked(SlotTag);
	RuntimeSlot.SlotTag = SlotTag;
	RuntimeSlot.ItemInstance = ItemInstance;
	RuntimeSlot.EquipmentInstance = Result.Instance;

	// Apply any slot blocking from the equipment policy
	if (const USlotPolicy* Policy = Definition->SlotPolicy)
	{
		LockSlots(Policy->GetBlockedSlotTags());
	}

	return Result;
}

#undef LOCTEXT_NAMESPACE
