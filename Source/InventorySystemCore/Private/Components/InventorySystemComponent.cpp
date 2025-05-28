// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Components/InventorySystemComponent.h"

#include "GameplayTagContainer.h"
#include "Containers/InventoryContainer.h"
#include "Data/InventoryCache.h"
#include "Data/InventoryEntry.h"
#include "Data/InventorySet.h"
#include "Engine/ActorChannel.h"
#include "GameplayTags/InventoryGameplayTags.h"
#include "Instances/ItemInstance.h"

UInventorySystemComponent::UInventorySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.Get())
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UInventorySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UInventorySystemComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bReplicated = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (const auto& [Tag, Container] : Containers)
	{
		if (IsValid(Container))
		{
			bReplicated |= Channel->ReplicateSubobject(Container, *Bunch, *RepFlags);
			bReplicated |= Container->ReplicateSubobjects(Channel, Bunch, RepFlags);
		}
	}

	return bReplicated;
}

void UInventorySystemComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register all item instances ready for replication with the actor component
	if (IsUsingRegisteredSubObjectList())
	{
		for (const auto& [Tag, Container] : Containers)
		{
			if (IsValid(Container))
			{
				AddReplicatedSubObject(Container);

				FInventoryList& InventoryList = Container->GetInventoryList();
				for (const FInventoryEntry& Entry : InventoryList.Entries)
				{
					if (UItemInstance* Instance = Entry.Instance; IsValid(Instance))
					{
						AddReplicatedSubObject(Instance);
					}
				}
			}
		}
	}
}

void UInventorySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// Cache initialization
	Cache = NewObject<UInventoryCache>(this);

	if (IsValid(DefaultContainerClass) && !Containers.Contains(DefaultContainerTag) && IsValidContainerTag(DefaultContainerTag))
	{
		UInventoryContainer* DefaultContainer = NewObject<UInventoryContainer>(this, DefaultContainerClass);
		RegisterContainer(DefaultContainerTag, DefaultContainer);
	}

	if (IsValid(DefaultInventorySet))
	{
		DefaultInventorySet->GiveToInventorySystem(this);
	}
}

void UInventorySystemComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
}

FInventoryResult UInventorySystemComponent::TryAddItemDefinitionIn(const FGameplayTag& ContainerTag, const TSubclassOf<UItemDefinition> ItemDefinition, const int32 Count)
{
	if (UInventoryContainer* Container = GetContainer(ContainerTag))
	{
		FInventoryResult Result = Container->TryAddItemDefinition(ItemDefinition, Count);
		if (Result.Succeeded() && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			for (UItemInstance* Instance : Result.Instances)
			{
				if (!IsReplicatedSubObjectRegistered(Instance))
				{
					AddReplicatedSubObject(Instance);
				}
			}
		}
		return Result;
	}
	return {{}, InventorySystemGameplayTags::TAG_Inventory_Failure_ContainerNotFound};
}

FInventoryResult UInventorySystemComponent::TryAddItemInstanceIn(const FGameplayTag& ContainerTag, UItemInstance* ItemInstance, const int32 StackCount)
{
	if (UInventoryContainer* Container = GetContainer(ContainerTag))
	{
		FInventoryResult Result = Container->TryAddItemInstance(ItemInstance, StackCount);
		if (Result.Succeeded() && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			for (UItemInstance* Instance : Result.Instances)
			{
				if (!IsReplicatedSubObjectRegistered(Instance))
				{
					AddReplicatedSubObject(Instance);
				}
			}
		}
		return Result;
	}
	return {{}, InventorySystemGameplayTags::TAG_Inventory_Failure_ContainerNotFound};
}

FInventoryResult UInventorySystemComponent::TryAddItemDefinition(const TSubclassOf<UItemDefinition>& ItemDefinition, const int32 Count)
{
	return TryAddItemDefinitionIn(DefaultContainerTag, ItemDefinition, Count);
}

FInventoryResult UInventorySystemComponent::TryAddItemInstance(UItemInstance* ItemInstance, const int32 StackCount)
{
	return TryAddItemInstanceIn(DefaultContainerTag, ItemInstance, StackCount);
}

bool UInventorySystemComponent::TryRemoveFromHandle(FInventoryEntryHandle Handle, FGameplayTag& OutFailureReason)
{
	if (!Handle.IsHandleValid() || !IsValid(Handle.Container))
	{
		OutFailureReason = InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidHandle;
		return false;
	}

	const bool Succeed = Handle.Container->TryRemoveItem(Handle, OutFailureReason);
	if (Succeed && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		RemoveReplicatedSubObject(Handle.ItemInstance);
	}
	return Succeed;
}

FInventoryResult UInventorySystemComponent::TryMoveByHandle(const FInventoryEntryHandle Handle, UInventoryContainer* TargetContainer)
{
	if (!Handle.IsHandleValid() || !IsValid(Handle.Container) || !IsValid(TargetContainer))
	{
		return {{}, InventorySystemGameplayTags::TAG_Inventory_Failure_InvalidHandle};
	}
	return Handle.Container->TryMoveItemTo(Handle, TargetContainer);
}

void UInventorySystemComponent::Empty()
{
	if (!IsUsingRegisteredSubObjectList())
	{
		return;
	}

	TArray<FInventoryEntryHandle> Handles;
	for (const auto& Pair : Containers)
	{
		if (UInventoryContainer* Container = Pair.Value)
		{
			FInventoryList& InventoryList = Container->GetInventoryList();
			for (const FInventoryEntry& Entry : InventoryList.Entries)
			{
				if (UItemInstance* Instance = Entry.Instance; IsValid(Instance))
				{
					RemoveReplicatedSubObject(Instance);
				}
			}
			Container->GetInventoryList().Entries.Empty();
		}
	}
}

FInventoryEntryHandle UInventorySystemComponent::FindHandleFromInstanceIn(const FGameplayTag& ContainerTag, UItemInstance* Instance) const
{
	for (const auto& Pair : Containers)
	{
		if (const UInventoryContainer* Container = Pair.Value)
		{
			FInventoryEntryHandle Handle = Container->FindHandle(Instance);
			if (Handle.IsHandleValid())
			{
				return Handle;
			}
		}
	}
	return FInventoryEntryHandle();
}

FInventoryEntryHandle UInventorySystemComponent::FindHandleFromInstance(UItemInstance* Instance) const
{
	return FindHandleFromInstanceIn(InventorySystemGameplayTags::TAG_Inventory_Container, Instance);
}

TArray<FInventoryEntryHandle> UInventorySystemComponent::GetAllStacks() const
{
	TArray<FInventoryEntryHandle> Handles;
	for (const auto& Pair : Containers)
	{
		if (UInventoryContainer* Container = Pair.Value)
		{
			Handles.Append(Container->GetInventoryList().GetAllHandles());
		}
	}
	return Handles;
}

int32 UInventorySystemComponent::GetStackCountByDefinitionIn(const TSubclassOf<UItemDefinition> DefinitionClass, const FGameplayTag& ContainerTag) const
{
	int32 Count = 0;
	if (const UInventoryContainer* Container = GetContainer(ContainerTag); IsValid(Container))
	{
		Count += Container->GetStackCountByDefinition(DefinitionClass);
	}
	return Count;
}

int32 UInventorySystemComponent::GetTotalCountByDefinitionIn(const TSubclassOf<UItemDefinition> DefinitionClass, const FGameplayTag& ContainerTag) const
{
	int32 Count = 0;
	if (const UInventoryContainer* Container = GetContainer(ContainerTag); IsValid(Container))
	{
		Count += Container->GetTotalCountByDefinition(DefinitionClass);
	}
	return Count;
}

int32 UInventorySystemComponent::GetStackCountByDefinition(const TSubclassOf<UItemDefinition> DefinitionClass) const
{
	int32 Count = 0;
	for (const auto& Pair : Containers)
	{
		Count += GetStackCountByDefinitionIn(DefinitionClass, Pair.Key);
	}
	return Count;
}

int32 UInventorySystemComponent::GetTotalCountByDefinition(const TSubclassOf<UItemDefinition> DefinitionClass) const
{
	int32 Count = 0;
	for (const auto& Pair : Containers)
	{
		Count += GetTotalCountByDefinitionIn(DefinitionClass, Pair.Key);
	}
	return Count;
}

bool UInventorySystemComponent::RegisterContainer(const FGameplayTag& Tag, UInventoryContainer* Container)
{
	if (!IsValid(Container) || !Tag.IsValid() || !IsValidContainerTag(Tag))
	{
		return false;
	}
	Container->SetOwnerComponent(this);

	Containers.Add(Tag, Container);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		if (!IsReplicatedSubObjectRegistered(Container))
		{
			AddReplicatedSubObject(Container);
		}
	}
	return true;
}

bool UInventorySystemComponent::UnregisterContainer(const FGameplayTag& Tag)
{
	if (!Tag.IsValid() || !Containers.Contains(Tag))
	{
		return false;
	}

	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		if (UInventoryContainer* Container = Containers.FindRef(Tag))
		{
			RemoveReplicatedSubObject(Container);
		}
	}
	Containers.Remove(Tag);
	return true;
}

UInventoryContainer* UInventorySystemComponent::GetContainer(const FGameplayTag& ContainerTag) const
{
	if (const TObjectPtr<UInventoryContainer>* Container = Containers.Find(ContainerTag))
	{
		return *Container;
	}
	return nullptr;
}

TMap<FGameplayTag, UInventoryContainer*> UInventorySystemComponent::GetAllContainers() const
{
	TMap<FGameplayTag, UInventoryContainer*> Out;
	for (const auto& Pair : Containers)
	{
		Out.Add(Pair.Key, Pair.Value);
	}
	return Out;
}


UItemDefinition* UInventorySystemComponent::GetCachedDefinition(const TSubclassOf<UItemDefinition>& Class) const
{
	if (IsValid(Cache))
	{
		return Cache->GetCachedDefinition(Class);
	}
	if (IsValid(Class))
	{
		return NewObject<UItemDefinition>(GetOuter(), Class);
	}
	return nullptr;
}

bool UInventorySystemComponent::IsValidContainerTag(const FGameplayTag& Tag)
{
	return Tag.MatchesTag(InventorySystemGameplayTags::TAG_Inventory_Container);
}

void UInventorySystemComponent::PostInventoryEntryAdded(const FInventoryChangeData& Data)
{
	OnInventoryEntryAdded.Broadcast(Data);
}

void UInventorySystemComponent::PostInventoryEntryRemoved(const FInventoryChangeData& Data)
{
	OnInventoryEntryRemoved.Broadcast(Data);
}

void UInventorySystemComponent::PostInventoryEntryChanged(const FInventoryChangeData& Data)
{
	OnInventoryEntryChanged.Broadcast(Data);
}

void UInventorySystemComponent::PostInventoryChanged(const FInventoryChangeData& Data)
{
	OnInventoryChanged.Broadcast(Data);
}
