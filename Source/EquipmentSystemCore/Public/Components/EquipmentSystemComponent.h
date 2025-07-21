// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Data/EquipmentCache.h"
#include "Data/EquipmentList.h"
#include "Data/Slots/DynamicEquipmentSlot.h"
#include "Instances/EquipmentInstance.h"

#include "EquipmentSystemComponent.generated.h"

class AWeaponActor;
class UItemInstance;
class UEquipmentInstance;
class UEquipmentDefinition;
class UEquipmentSlotMapData;
class USlotPolicy;
struct FEquipmentChangeData;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChange, const FEquipmentChangeData&, Data);

/**
 * @class UEquipmentSystemComponent
 * @see UActorComponent
 * @brief Component managing equipment system functionality for actors
 * @details Manages equipment slots, item equipping/unequipping, and equipment instances.
 * Provides runtime validation of equipment changes through policies, maintains a dynamic
 * slot system, and handles replication of equipped items. Supports blocking/unblocking
 * equipment slots and broadcasting equipment change events.
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class EQUIPMENTSYSTEMCORE_API UEquipmentSystemComponent : public UActorComponent
{
	GENERATED_BODY()

	friend FEquipmentList;

public:
	explicit UEquipmentSystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	// ~UObject

	// AActorComponent
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	// ~AActorComponent


	/**
	 * Attempts to equip an item in the first available slot
	 * @param ItemInstance The item to equip
	 * @return True if the item was successfully equipped
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FEquipmentResult TryEquipItem(UItemInstance* ItemInstance);

	/**
	 * Attempts to equip an item in a specific slot
	 * @param ItemInstance The item to equip
	 * @param SlotTag The target slot to equip the item in
	 * @return True if the item was successfully equipped
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	FEquipmentResult TryEquipItemOnSlot(UItemInstance* ItemInstance, const FGameplayTag& SlotTag);

	/**
	 * Attempts to unequip a currently equipped item
	 * @param ItemInstance The item to unequip
	 * @param OutFailureReason Contains the reason if the operation fails
	 * @return True if the item was successfully unequipped
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool TryUnequipItem(UItemInstance* ItemInstance, FGameplayTag& OutFailureReason);

	/**
	 * Attempts to unequip whatever item is in the specified slot
	 * @param SlotTag The slot to unequip
	 * @param OutFailureReason Contains the reason if the operation fails
	 * @return True if the slot was successfully unequipped
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool TryUnequipSlot(const FGameplayTag SlotTag, FGameplayTag& OutFailureReason);

	/**
	 * Attempts to swap items between two equipment slots
	 * @param SlotA First slot to swap
	 * @param SlotB Second slot to swap
	 * @param OutFailureReason Contains the reason if the operation fails
	 * @return True if the slots were successfully swapped
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool TrySwapSlots(const FGameplayTag& SlotA, const FGameplayTag& SlotB, FGameplayTag& OutFailureReason);


	/**
	 * Checks if a slot is currently blocked from equipment changes
	 * @param SlotTag The slot to check
	 * @param bExact If true, only checks for exact tag match
	 * @return True if the slot is blocked
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment|Slots")
	bool IsSlotBlocked(const FGameplayTag& SlotTag, bool bExact = false) const;

	/**
	 * Finds a slot by its tag
	 * @param SlotTag The tag of the slot to find
	 * @return Pointer to the found slot, or nullptr if not found
	 */
	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment|Slots")
	const FDynamicEquipmentSlot* FindSlot(const FGameplayTag& SlotTag) const;

	/**
	 * Gets the current runtime slot map
	 * @return Map of all equipment slots
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment|Slots")
	TMap<FGameplayTag, FDynamicEquipmentSlot> GetSlotMap() const { return SlotMapRuntime; }


	/**
	 * Gets the slot tag where an item is currently equipped
	 * @param ItemInstance The item to find the slot for
	 * @return The tag of the slot containing the item
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment|Slots")
	const FGameplayTag& GetSlotForItem(UItemInstance* ItemInstance);

	/**
	 * Gets the item instance equipped in a specific slot
	 * @param SlotTag The slot to check
	 * @return The item in the slot, or nullptr if empty
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment|Slots")
	UItemInstance* GetItemInSlot(const FGameplayTag& SlotTag);

	/**
	 * Gets the equipment instance in a specific slot
	 * @param SlotTag The slot to check
	 * @return The equipment in the slot, or nullptr if empty
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment|Slots")
	UEquipmentInstance* GetEquipmentInSlot(const FGameplayTag& SlotTag);

	/**
	 * Gets all currently equipped items
	 * @param OutItems Map of slot tags to equipped items
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	void GetEquippedItems(TMap<FGameplayTag, UItemInstance*>& OutItems) const;


	/**
	 * Gets an instance of the specified equipment type.
	 * @param InstanceClass The class of the equipment instance to find.
	 * @return The first found UEquipmentInstance of the specified type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = InstanceClass))
	UEquipmentInstance* GetInstanceOfType(const TSubclassOf<UEquipmentInstance>& InstanceClass);
	/**
	 * Gets an instance of specified equipment type using templates
	 * @tparam T The equipment instance type to find
	 * @return The first found instance of type T, or nullptr if none found
	 */
	template <typename T>
	T* GetInstanceOfType() const;

	/**
	 * Gets all instances of the specified equipment type.
	 * @param InstanceClass The class of the equipment instances to find.
	 * @return An array of UEquipmentInstances of the specified type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = InstanceClass))
	TArray<UEquipmentInstance*> GetAllInstancesOfType(const TSubclassOf<UEquipmentInstance>& InstanceClass);
	/**
	 * Gets all instances of specified equipment type using templates
	 * @tparam T The equipment instance type to find
	 * @return Array of all instances of type T
	 */
	template <typename T>
	TArray<T*> GetAllInstancesOfType() const;

	/**
	 * Retrieves a cached equipment definition
	 * @param Class The class of equipment definition to retrieve
	 * @return The cached equipment definition instance, or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, Category = "Equipment", meta = (DeterminesOutputType = Class))
	UEquipmentDefinition* GetCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class) const;

	/**
	 * Gets the equipment instance of the associated ItemInstance
	 * @param ItemInstance The instance of the source item
	 * @return The equipment instance associated with the item, or nullptr if not found
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	UEquipmentInstance* GetInstanceFromItem(UItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	AWeaponActor* FindWeaponActorAttachedToBone(const FName& BoneName) const;
	
protected:
	/**
	 * Called after equipment is successfully equipped
	 * @param Data The equipment change data
	 */
	virtual void PostEquipmentEquipped(const FEquipmentChangeData& Data);

	/**
	 * Called after equipment is successfully unequipped
	 * @param Data The equipment change data
	 */
	virtual void PostEquipmentUnequipped(const FEquipmentChangeData& Data);

	/**
	 * Called after any equipment change occurs
	 * @param Data The equipment change data
	 */
	virtual void PostEquipmentChanged(const FEquipmentChangeData& Data);

	/**
	 * Locks specified equipment slots from being modified
	 * @param Tags Container of slot tags to lock
	 */
	void LockSlots(const FGameplayTagContainer& Tags);

	/**
	 * Unlocks previously locked equipment slots
	 * @param Tags Container of slot tags to unlock
	 */
	void UnlockSlots(const FGameplayTagContainer& Tags);

	// Internal Equipment Logic
	FEquipmentResult Internal_ProcessEquip(UItemInstance* ItemInstance, const FGameplayTag& TargetSlot, const UEquipmentDefinition* Definition);
	bool Internal_ProcessUnequip(const FGameplayTag& SlotTag, FGameplayTag& OutFailureReason, bool bPreserveItem = false);
	FEquipmentResult Internal_EquipOnSlot(const FGameplayTag& SlotTag, UItemInstance* ItemInstance, const UEquipmentDefinition* Definition);

public:
	/** Delegate dispatched when an item is equipped */
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentChange OnEquipmentChanged;
	/** Broadcasts when equipment is equipped, providing the instance. */
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentChange OnEquipmentEquipped;
	/** Broadcasts when equipment is unequipped, providing the instance. */
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentChange OnEquipmentUnequipped;

protected:
	/** The list of equipped items. */
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;

	/** Equipment definitions cache. Not replicated */
	UPROPERTY()
	TObjectPtr<UEquipmentCache> Cache;

	UPROPERTY(EditDefaultsOnly, Category = "Slots")
	TObjectPtr<UEquipmentSlotMapData> SlotMapData;

	UPROPERTY()
	TMap<FGameplayTag, FDynamicEquipmentSlot> SlotMapRuntime;

	FGameplayTagCountContainer BlockedSlots;
};

template <typename T>
T* UEquipmentSystemComponent::GetInstanceOfType() const
{
	return Cast<T>(GetInstanceOfType(T::StaticClass()));
}

template <typename T>
TArray<T*> UEquipmentSystemComponent::GetAllInstancesOfType() const
{
	TArray<T*> Instances;
	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA<T>())
			{
				Instances.Add(Cast<T>(Instance));
			}
		}
	}
	return Instances;
}
