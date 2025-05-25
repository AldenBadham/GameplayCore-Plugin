// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Containers/InventoryContainer.h"
#include "Data/InventoryCache.h"
#include "Data/InventoryList.h"
#include "Definitions/ItemDefinition.h"
#include "GameplayTags/InventoryGameplayTags.h"

#include "InventorySystemComponent.generated.h"

class UInventorySet;
struct FGameplayTag;
class UEquipmentComponent;


/**
 * Multicast delegate that broadcasts inventory change events to all listeners
 * @param Data Detailed information about the inventory change including the affected item, previous state, and type of change
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChange, const FInventoryChangeData&, Data);

/**
 * @class UInventorySystemComponent
 * @see UActorComponent
 * @brief A component that manages an actor's inventory system with network replication support
 * @details Provides a flexible inventory system that can store, manage, and replicate items.
 * Supports dynamic item addition/removal, quantity tracking, and cache management for optimal performance.
 * Implements a robust replication system for multiplayer games. All operations are replicated to clients
 * and changes can be tracked through delegates.
 */

UCLASS(BlueprintType, ClassGroup = ("Equipment"), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEMCORE_API UInventorySystemComponent : public UActorComponent
{
	GENERATED_BODY()

	friend FInventoryList;

public:
	UInventorySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	// ~UObject

	// AActorComponent
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	// ~AActorComponent


	UFUNCTION(BlueprintCallable, Category="Inventory", meta = (Categories = "Inventory.Container"))
	FInventoryResult TryAddItemDefinitionIn(const FGameplayTag& ContainerTag, TSubclassOf<UItemDefinition> ItemDefinition, int32 Count);
	UFUNCTION(BlueprintCallable, Category="Inventory", meta = (Categories = "Inventory.Container"))
	FInventoryResult TryAddItemInstanceIn(const FGameplayTag& ContainerTag, UItemInstance* ItemInstance, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	FInventoryResult TryAddItemDefinition(const TSubclassOf<UItemDefinition>& ItemDefinition, int32 Count);
	UFUNCTION(BlueprintCallable, Category="Inventory")
	FInventoryResult TryAddItemInstance(UItemInstance* ItemInstance, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool TryRemoveFromHandle(FInventoryEntryHandle Handle, FGameplayTag& OutFailureReason);
	UFUNCTION(BlueprintCallable, Category="Inventory")
	FInventoryResult TryMoveByHandle(FInventoryEntryHandle Handle, UInventoryContainer* TargetContainer);


	UFUNCTION(BlueprintCallable, Category="Inventory|Query", meta = (Categories = "Inventory.Container"))
	FInventoryEntryHandle FindHandleFromInstanceIn(const FGameplayTag& ContainerTag, UItemInstance* Instance) const;
	UFUNCTION(BlueprintCallable, Category="Inventory|Query")
	FInventoryEntryHandle FindHandleFromInstance(UItemInstance* Instance) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FInventoryEntryHandle> GetAllStacks() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (Categories = "Inventory.Container"))
	int32 GetStackCountByDefinitionIn(TSubclassOf<UItemDefinition> DefinitionClass, const FGameplayTag& ContainerTag) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (Categories = "Inventory.Container"))
	int32 GetTotalCountByDefinitionIn(TSubclassOf<UItemDefinition> DefinitionClass, const FGameplayTag& ContainerTag) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetStackCountByDefinition(TSubclassOf<UItemDefinition> DefinitionClass) const;
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetTotalCountByDefinition(TSubclassOf<UItemDefinition> DefinitionClass) const;


	UFUNCTION(BlueprintCallable, Category="Inventory|Container", meta = (Categories = "Inventory.Container"))
	bool RegisterContainer(const FGameplayTag& Tag, UInventoryContainer* Container);
	UFUNCTION(BlueprintCallable, Category="Inventory|Container", meta = (Categories = "Inventory.Container"))
	bool UnregisterContainer(const FGameplayTag& Tag);
	UFUNCTION(BlueprintPure, Category="Inventory|Container", meta = (Categories = "Inventory.Container"))
	UInventoryContainer* GetContainer(const FGameplayTag& ContainerTag) const;
	UFUNCTION(BlueprintPure, Category="Inventory|Container")
	TMap<FGameplayTag, UInventoryContainer*> GetAllContainers() const;


	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (DeterminesOutputType = DefinitionClass))
	UItemDefinition* GetCachedDefinition(const TSubclassOf<UItemDefinition>& DefinitionClass) const;

public:
	
protected:
	
	static bool IsValidContainerTag(const FGameplayTag& Tag);
	
	/**
	 * Called after an item is added to the inventory
	 * @param Data Information about the added inventory entry
	 */
	virtual void PostInventoryEntryAdded(const FInventoryChangeData& Data);

	/**
	 * Called after an item is removed from the inventory
	 * @param Data Information about the removed inventory entry
	 */
	virtual void PostInventoryEntryRemoved(const FInventoryChangeData& Data);

	/**
	 * Called after an inventory entry is modified
	 * @param Data Information about the modified inventory entry
	 */
	virtual void PostInventoryEntryChanged(const FInventoryChangeData& Data);

	/**
	 * Called after any change to the inventory
	 * @param Data Information about the inventory change
	 */
	virtual void PostInventoryChanged(const FInventoryChangeData& Data);

protected:
	
	/** Event fired when an item is added to the inventory */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChange OnInventoryEntryAdded;

	/** Event fired when an item is removed from the inventory */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChange OnInventoryEntryRemoved;

	/** Event fired when an inventory entry is modified */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChange OnInventoryEntryChanged;

	/** Event fired after any change to the inventory */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChange OnInventoryChanged;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryContainer> DefaultContainerClass = UInventoryContainer::StaticClass();
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (Categories = "Inventory.Container"))
	FGameplayTag DefaultContainerTag = InventorySystemGameplayTags::TAG_Inventory_Container_Default;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UInventorySet> DefaultInventorySet;
	
	UPROPERTY(/* Replicated */) // Should be marked as replicated but not supported, so replicated as subobjects
	TMap<FGameplayTag, TObjectPtr<UInventoryContainer>> Containers;

	/** Inventory definitions cache. Not replicated */
	UPROPERTY()
	TObjectPtr<UInventoryCache> Cache;
};
