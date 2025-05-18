// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Data/InventoryList.h"
#include "Data/InventoryCache.h"
#include "Definitions/ItemDefinition.h"

#include "InventorySystemComponent.generated.h"

class UEquipmentComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChange, FInventoryChangeData, Data);

UCLASS(BlueprintType, ClassGroup = ("Equipment"), meta = (BlueprintSpawnableComponent))
class INVENTORYSYSTEMCORE_API UInventorySystemComponent : public UActorComponent
{
	GENERATED_BODY()

	friend FInventoryList;

public:
	
	UInventorySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual ~UInventorySystemComponent() override;

	// UObject
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	// ~UObject

	/**
	 * Adds an item definition to the inventory, creating an instance.
	 * @param ItemDefinition The class of the item definition to add.
	 * @param Count The number of instances to add (default is 1).
	 * @return The instance of the added item, or nullptr if unsuccessful.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	UItemInstance* AddItemDefinition(TSubclassOf<UItemDefinition> ItemDefinition, int32 Count = 1);

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
	 * Retrieves the total count of items in the inventory based on their definition.
	 * @param ItemDefinitionClass The class of the item definition to count.
	 * @return The total count of items matching the specified definition.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory") // BlueprintAuthorityOnly
	int32 GetTotalItemCountByDefinition(TSubclassOf<UItemDefinition> ItemDefinitionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta=(DeterminesOutputType = ItemDefinitionClass))
	UItemDefinition* GetItemDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const;
	
	virtual void PostInventoryEntryAdded(const FInventoryChangeData& Data);
	virtual void PostInventoryEntryRemoved(const FInventoryChangeData& Data);
	virtual void PostInventoryEntryChanged(const FInventoryChangeData& Data);
	virtual void PostInventoryChanged(const FInventoryChangeData& Data);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChange OnInventoryEntryAdded;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChange OnInventoryEntryRemoved;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChange OnInventoryEntryChanged;
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChange OnInventoryChanged;

private:
	/** Replicated list of inventory entries managed by this component. */
	UPROPERTY(Replicated)
	FInventoryList InventoryList;

	/** Inventory definitions cache. Not replicated */
	UPROPERTY()
	TWeakObjectPtr<UInventoryCache> Cache;
};
