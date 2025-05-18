// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ItemDefinition.generated.h"

class UInventorySystemComponent;
class UItemFragment;

/**
 * @class UItemDefinition
 * @see UObject
 * @brief Data asset definition of an item that can be given/owned by a character
 * @details Represents a template for items in the inventory system. Contains basic item information 
 * like name and description, and can be extended with fragments for additional functionality.
 * This is an abstract class that should be inherited to create specific item types.
 */
UCLASS(Blueprintable, Abstract, BlueprintType)
class INVENTORYSYSTEMCORE_API UItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// ~UObject

	/**
	 * Searches for a specific fragment type in this item definition
	 * @param FragmentClass The class type of fragment to find
	 * @return The found fragment instance, or nullptr if not found
	 * @see UItemFragment
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;

	/**
	 * Checks if this item definition contains a specific fragment type
	 * @param FragmentClass The class type of fragment to check
	 * @return True if the fragment exists, false otherwise
	 * @see UItemFragment
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	bool HasFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;
	/**
	 * Template version of FindFragmentByClass for convenient type-safe fragment access
	 * @tparam T The specific fragment class type to find
	 * @return Typed pointer to the found fragment, or nullptr if not found
	 * @see FindFragmentByClass
	 */
	template <typename T> const T* FindFragmentByClass() const { return Cast<T>(FindFragmentByClass(T::StaticClass())); }

	/**
	 * Checks if this item can be given to the specified inventory
	 * @param InventorySystemComponent The target inventory component
	 * @return True if the item can be given, false otherwise
	 * @see UInventorySystemComponent
	 */
	virtual bool CanBeGiven(UInventorySystemComponent* InventorySystemComponent);
	
	/**
	 * Blueprint accessible version of CanBeGiven
	 * @param InventorySystemComponent The target inventory component
	 * @return True if the item can be given, false otherwise
	 * @see CanBeGiven
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
	bool K2_CanBeGiven(UInventorySystemComponent* InventorySystemComponent);

	// TODO : Add item categories

	/** The display name of the item shown in the UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition")
	FText DisplayName;
	
	/** Detailed description of the item shown in the UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition")
	FText Description;
	
	/** Collection of fragments that define the item's behavior and properties */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments", Instanced)
	TArray<TObjectPtr<UItemFragment>> Fragments;

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	TArray<UItemFragment*> PreviousFragments;
#endif
};
