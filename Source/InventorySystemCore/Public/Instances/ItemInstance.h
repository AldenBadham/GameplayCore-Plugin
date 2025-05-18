// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/InventorySystemComponent.h"
#include "Components/ItemComponent.h"
#include "CoreMinimal.h"
#include "Definitions/ItemDefinition.h"
#include "UObject/Object.h"

#include "ItemInstance.generated.h"

struct FInventoryList;

/**
 * @class UItemInstance
 * @see UObject
 * @brief This class represents an instance of an item within the inventory system. It holds a reference
 * to the item definition and provides functionalities related to item management.
 * This class uses a component-based architecture to allow for flexible
 * and efficient item instances. Each instance can have multiple components
 * that add specific functionalities.
 */
UCLASS(BlueprintType, Blueprintable)
class INVENTORYSYSTEMCORE_API UItemInstance : public UObject
{
	GENERATED_BODY()

	friend struct FInventoryList;

public:
	explicit UItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	// ~UObject

	/**
	 * Gets the inventory system component that owns this item instance
	 * @return The owning inventory system component
	 */
	UFUNCTION(BlueprintCallable, Category = "Instance")
	UInventorySystemComponent* GetInventorySystemComponent() const;
	
	/**
	 * Gets the player controller that owns this item instance
	 * @return The owning player controller
	 */
	UFUNCTION(BlueprintCallable, Category = "Instance")
	APlayerController* GetOwningController() const;
	/**
	 * Gets the owning controller cast to the specified type
	 * @return The owning controller cast to type T, or nullptr if cast fails
	 */
	template <typename T> const T* GetOwningController() const { return Cast<T>(GetOwningController(T::StaticClass())); }

	/**
	 * Gets the item definition class associated with this instance.
	 * @return The item definition class.
	 */
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UItemDefinition> GetDefinitionClass() const;

	/**
	 * Gets the item definition associated with this instance, cached by the Inventory System Component.
	 * @return The item definition default object.
	 */
	UFUNCTION(BlueprintCallable)
	UItemDefinition* GetDefinition() const;

	/**
	 * Try to find fragment of class FragmentClass in this item's definition
	 * @param FragmentClass Class of the ItemFragment to search
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;
	/**
	 * Try to find fragment of class FragmentClass in this item's definition
	 */
	template <typename T> const T* FindFragmentByClass() const { return Cast<T>(FindFragmentByClass(T::StaticClass())); }

	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	bool HasFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;

	/**
	 *	Try to find fragment of class FragmentClass in this item definition
	 *	@param ComponentClass Class of the ItemFragment to search
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	UItemComponent* AddComponent(TSubclassOf<UItemComponent> ComponentClass);
	/**
	 *	Try to find fragment of class FragmentClass in this item definition
	 */
	template <typename T> T* AddComponent() { return Cast<T>(AddComponent(T::StaticClass())); }

	/**
	 *	Try to find component of class ComponentClass of this item instance
	 *	@param ComponentClass Class of the Item Instance's component to search
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = ComponentClass))
	const UItemComponent* FindComponentByClass(TSubclassOf<UItemComponent> ComponentClass) const;
	/**
	 * Try to find a component of type T attached to this item instance
	 * @return The found component cast to type T, or nullptr if not found or cast fails
	 */
	template <typename T> const T* FindComponentByClass() const { return Cast<T*>(FindComponentByClass(T::StaticClass())); }

protected:
	/**
	 * Sets the item definition class for this instance.
	 * @param InDefinition - The item definition to set.
	 */
	void SetDefinition(UItemDefinition* InDefinition);

	/** The item definition that this instance is based on.
	 * Only replicate the class.
	 */
	UPROPERTY(Replicated)
	TSubclassOf<UItemDefinition> DefinitionClass;

	/** The Item Definition default object, cached by the local InventorySystemComponent cache */
	UPROPERTY()
	TWeakObjectPtr<UItemDefinition> Definition;

	/** Components attached to this item instance providing additional functionality */
	UPROPERTY(Replicated)
	TArray<UItemComponent*> Components;

	/** Cached pointer to the player controller that owns this item instance */
	UPROPERTY(Transient)
	mutable APlayerController* OwningController;
};
