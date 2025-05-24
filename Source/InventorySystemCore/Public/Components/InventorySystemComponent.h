// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Data/InventoryCache.h"
#include "Data/InventoryList.h"
#include "Definitions/ItemDefinition.h"

#include "InventorySystemComponent.generated.h"

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
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	// ~UObject

	/**
	 * Creates and adds new item instances to the inventory based on an item definition
	 * @param ItemDefinition The class definition of the item to create and add
	 * @param Count The quantity of items to add (must be positive)
	 * @return Array of created item instances, empty if addition failed or ItemDefinition is invalid
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	TArray<UItemInstance*> AddItemDefinition(TSubclassOf<UItemDefinition> ItemDefinition, int32 Count = 1);

	/**
	 * Adds an existing item instance to the inventory.
	 * @param ItemInstance The instance of the item to add.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void AddItemInstance(UItemInstance* ItemInstance);
	/**
	 * Removes an item instance from the inventory.
	 * @param ItemInstance The instance of the item to remove.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void RemoveItemInstance(UItemInstance* ItemInstance);

	/**
	 * Retrieves all items currently in the inventory.
	 * @return An array containing all item instances in the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory") // BlueprintAuthorityOnly
	TArray<FInventoryEntryHandle> GetAllStacks() const;

	/**
	 * Calculates the sum of all stack counts for items matching the specified definition
	 * @param DefinitionClass The class of items to count
	 * @return Total quantity of all matching items across all stacks
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetStackCountByDefinition(TSubclassOf<UItemDefinition> DefinitionClass) const;

	/**
	 * Counts the number of separate stacks containing items of the specified definition
	 * @param DefinitionClass The class of items to count stacks for
	 * @return Number of inventory slots containing the specified item type
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetTotalCountByDefinition(TSubclassOf<UItemDefinition> DefinitionClass) const;

	/**
	 * Retrieves an item definition from the inventory cache
	 * @param DefinitionClass The class of the item definition to retrieve
	 * @return The item definition instance or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory", meta = (DeterminesOutputType = DefinitionClass))
	UItemDefinition* GetCachedDefinition(const TSubclassOf<UItemDefinition>& DefinitionClass) const;

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

private:
	/** Replicated list of inventory entries managed by this component. */
	UPROPERTY(Replicated)
	FInventoryList InventoryList;

	/** Inventory definitions cache. Not replicated */
	UPROPERTY()
	TObjectPtr<UInventoryCache> Cache;
};
