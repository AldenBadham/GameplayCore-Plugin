// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Data/EquipmentCache.h"
#include "Data/EquipmentList.h"
#include "EquipmentSystemComponent.generated.h"

class UEquipmentInstance;

/**
 * @class UEquipmentSystemComponent
 * @see UActorComponent
 * @brief This component manages the equipment system for an actor.
 */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class EQUIPMENTSYSTEMCORE_API UEquipmentSystemComponent : public UActorComponent
{
	GENERATED_BODY()

	friend FEquipmentList;
	
public:
	
	explicit UEquipmentSystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual ~UEquipmentSystemComponent() override;

	// UObject
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	// ~UObject

	// UActorComponent
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	// ~UActorComponent
	
	/**
	 * Equips an item based on the provided equipment definition.
	 * @param EquipmentDefinition The class of the equipment to equip.
	 * @return The equipped UEquipmentInstance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UEquipmentInstance* EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	/**
	 * Unequips the specified item instance.
	 * @param ItemInstance The equipment instance to unequip.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UEquipmentInstance* ItemInstance);
	
	/**
	 * Gets an instance of the specified equipment type.
	 * @param InstanceClass The class of the equipment instance to find.
	 * @return The first found UEquipmentInstance of the specified type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	UEquipmentInstance* GetInstanceOfType(TSubclassOf<UEquipmentInstance> InstanceClass);
	/**
	 * Gets all instances of the specified equipment type.
	 * @param InstanceClass The class of the equipment instances to find.
	 * @return An array of UEquipmentInstances of the specified type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	TArray<UEquipmentInstance*> GetAllEquipmentInstancesOfType(TSubclassOf<UEquipmentInstance> InstanceClass);

	UFUNCTION(BlueprintCallable, Category = "Inventory", meta=(DeterminesOutputType = Class))
	UEquipmentDefinition* GetEquipmentDefinition(const TSubclassOf<UEquipmentDefinition>& Class) const;
	
	/**
	 * Gets the equipment instance of the associated ItemInstance
	 * @param ItemInstance The instance of the source item
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	UEquipmentInstance* GetInstanceFromItem(UItemInstance* ItemInstance);

	/**
	 * Template function to get an instance of a specific equipment type.
	 * @return The first found instance of the specified type.
	 */
	template <typename T>
	T* GetInstanceOfType()
	{
		return Cast<T>(GetInstanceOfType(T::StaticClass()));
	}
protected:
	
    /** The list of equipped items. */
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;

	/** Equipment definitions cache. Not replicated */
	UPROPERTY()
	TWeakObjectPtr<UEquipmentCache> Cache;
};
