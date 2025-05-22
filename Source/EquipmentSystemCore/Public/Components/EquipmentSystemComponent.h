#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Data/EquipmentCache.h"
#include "Data/EquipmentList.h"
#include "Instances/EquipmentInstance.h"

#include "EquipmentSystemComponent.generated.h"

struct FEquipmentChangeData;
class UItemInstance;
class UEquipmentInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentChanged, const FEquipmentChangeData&, Data);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentEquipped, const FEquipmentChangeData&, Data);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentUnequipped, const FEquipmentChangeData&, Data);

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
	 * @param SourceItemDefinition
	 * @return The equipped UEquipmentInstance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UEquipmentInstance* EquipItem(const TSubclassOf<UEquipmentDefinition>& EquipmentDefinition, UItemInstance* SourceItemDefinition = nullptr);
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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = InstanceClass))
	UEquipmentInstance* GetInstanceOfType(const TSubclassOf<UEquipmentInstance>& InstanceClass);
	/**
	 * Template function to get an instance of a specific equipment type.
	 * @return The first found an instance of the specified type.
	 */
	template <typename T> T* GetInstanceOfType() const;

	/**
	 * Gets all instances of the specified equipment type.
	 * @param InstanceClass The class of the equipment instances to find.
	 * @return An array of UEquipmentInstances of the specified type.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = InstanceClass))
	TArray<UEquipmentInstance*> GetAllInstancesOfType(const TSubclassOf<UEquipmentInstance>& InstanceClass);

	template <typename T> TArray<T*> GetAllInstancesOfType() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment", meta = (DeterminesOutputType = Class))
	UEquipmentDefinition* GetCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class) const;

	/**
	 * Gets the equipment instance of the associated ItemInstance
	 * @param ItemInstance The instance of the source item
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	UEquipmentInstance* GetInstanceFromItem(UItemInstance* ItemInstance);

	virtual void PostEquipmentEquipped(const FEquipmentChangeData& Data);
	virtual void PostEquipmentUnequipped(const FEquipmentChangeData& Data);
	virtual void PostEquipmentChanged(const FEquipmentChangeData& Data);

	/** Broadcasts when equipment changes, providing both the new and old instance. */
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentChanged OnEquipmentChanged;
	/** Broadcasts when equipment is equipped, providing the instance. */
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentEquipped OnEquipmentEquipped;
	/** Broadcasts when equipment is unequipped, providing the instance. */
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentUnequipped OnEquipmentUnequipped;

protected:
	/** The list of equipped items. */
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;

	/** Equipment definitions cache. Not replicated */
	UPROPERTY()
	TObjectPtr<UEquipmentCache> Cache;
};

template <typename T> T* UEquipmentSystemComponent::GetInstanceOfType() const
{
	return Cast<T>(GetFirstInstanceOfType(T::StaticClass()));
}

template <typename T> TArray<T*> UEquipmentSystemComponent::GetAllInstancesOfType() const
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