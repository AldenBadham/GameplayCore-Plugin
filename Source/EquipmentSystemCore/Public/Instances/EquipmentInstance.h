// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "Components/EquipmentComponent.h"
#include "Data/EquipmentActorSet.h"
#include "Definitions/EquipmentDefinition.h"
#include "Engine/World.h"

#include "EquipmentInstance.generated.h"

class UEquipmentFragment;
/**
 * @class UEquipmentInstance
 * @see UObject
 * @brief Represents an instance of equipment that can be equipped by a pawn.
 */
UCLASS(BlueprintType, Blueprintable)
class EQUIPMENTSYSTEMCORE_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()

	friend struct FEquipmentList;
	friend class UEquipmentSlotComponent;

public:
	UEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// ~UObject

	/** Get the pawn that this equipment instance is attached to. */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	APawn* GetPawn() const;
	/** Get the pawn that this equipment instance is attached to, if applicable. */
	UFUNCTION(BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = PawnType))
	APawn* GetTypedPawn(const TSubclassOf<APawn>& PawnType) const;
	/** Get the pawn that this equipment instance is attached to, if applicable. */
	template <class T>
	T* GetPawn() const { return Cast<T>(GetPawn()); }

	/**
	 *	Add a new component object of class ComponentClass to this EquipmentInstance
	 *	@param ComponentClass Class of the EquipmentComponent to search
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	UEquipmentComponent* AddComponent(TSubclassOf<UEquipmentComponent> ComponentClass);
	/**
	 *	Add a new component object of class ComponentClass to this EquipmentInstance
	 */
	template <typename T>
	T* AddComponent() { return Cast<T>(AddComponent(T::StaticClass())); }

	/**
	 *	Try to find component of class ComponentClass of this equipment instance
	 *	@param ComponentClass Class of the Equipment Instance's component to search
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = ComponentClass))
	const UEquipmentComponent* FindComponentByClass(TSubclassOf<UEquipmentComponent> ComponentClass) const;
	/**
	 *	Try to find fragment of class ComponentClass of this equipment instance
	 */
	template <typename T>
	const T* FindComponentByClass() const { return Cast<T*>(FindComponentByClass(T::StaticClass())); }

	/** Get the instigator object that caused this equipment instance to be equipped. */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UObject* GetInstigator() const;
	/** Set the instigator object for this equipment instance. */
	void SetInstigator(UObject* InInstigator);

	/** Get the source object that caused this equipment instance to be equipped.  (ItemInstance class)*/
	UFUNCTION(BlueprintPure, Category = "Equipment")
	UItemInstance* GetSourceItem() const;

	/**
	 * Gets the equipment definition class associated with this instance.
	 * @return The equipment definition class.
	 */
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UEquipmentDefinition> GetDefinitionClass() const;

	/**
	 * Gets the equipment definition associated with this instance, cached by the Equipment System Component.
	 * @return The equipment definition default object.
	 */
	UFUNCTION(BlueprintCallable)
	UEquipmentDefinition* GetDefinition() const;

	/**
	 * Spawn actors defined in the given array and attach them to the pawn.
	 * @param ActorsToSpawn Array of actor sets defining which actors to spawn and how to attach them. */
	virtual void SpawnActors(const TArray<FEquipmentActorSet>& ActorsToSpawn);
	/** Destroy all actors spawned by this equipment instance. */
	virtual void DestroyActors();
	/** Get an array of actors spawned by this equipment instance. */
	UFUNCTION(BlueprintPure, Category = "Equipment")
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	/** Called the equipment instance is equipped. */
	virtual void OnEquipped();
	/** Called the equipment instance is unequipped. */
	virtual void OnUnequipped();

	/** Called the equipment instance is spawned. */
	virtual void OnSpawned();

protected:
	/** Set the source object for this equipment instance. (ItemInstance class)*/
	void SetSourceItem(UItemInstance* InSourceObject);

	/**
	 * Sets the item definition class for this instance.
	 * @param InDefinition - The item definition to set.
	 */
	void SetDefinition(UEquipmentDefinition* InDefinition);

	/** Blueprint event called when this equipment instance is equipped. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	/** Blueprint event called when this equipment instance is unequipped. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

	UFUNCTION()
	virtual void OnRep_Instigator();

	UPROPERTY(Replicated)
	TArray<AActor*> SpawnedActors;

	/** The Equipment definition that this instance is based on.
	 * Only replicate the class.
	 */
	UPROPERTY(Replicated)
	TSubclassOf<UEquipmentDefinition> DefinitionClass;

	/** The Equipment Definition default object, cached by the local EquipmentSystemComponent cache */
	UPROPERTY()
	TObjectPtr<UEquipmentDefinition> Definition;

	UPROPERTY()
	TObjectPtr<UItemInstance> SourceItem;

	/** Array of components attached to this item instance. */
	UPROPERTY(Replicated)
	TArray<UEquipmentComponent*> Components;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Instigator)
	TObjectPtr<UObject> Instigator;
};
