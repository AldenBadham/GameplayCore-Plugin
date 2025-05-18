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
 * @brief Data asset definition of an item that can be given/owned by a
 * character
 */
UCLASS(Blueprintable, Abstract, BlueprintType)
class INVENTORYSYSTEMCORE_API UItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	
	UItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// ~UObject
	
	/**
	 *	Try to find fragment of class FragmentClass in this item definition
	 *	@param FragmentClass Class of the ItemFragment to search
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
	const UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	bool HasFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;
	/**
	 *	Try to find fragment of class FragmentClass in this item definition
	 */
	template <typename T>
	const T* FindFragmentByClass() const
	{
		return Cast<T>(FindFragmentByClass(T::StaticClass()));
	}

	virtual bool CanGive(UInventorySystemComponent* InventorySystemComponent);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintPure)
	bool K2_CanGive(UInventorySystemComponent* InventorySystemComponent);

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Definition")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fragments", Instanced)
	TArray<TObjectPtr<UItemFragment>> Fragments;
};
