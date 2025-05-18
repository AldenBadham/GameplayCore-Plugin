// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ItemFragment.generated.h"

class UItemInstance;

/**
 * @class UItemFragment
 * @see UItemDefinition
 * @brief Represents a fragment of an item definition, allow definition extension.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType, Abstract)
class INVENTORYSYSTEMCORE_API UItemFragment : public UObject
{
	GENERATED_BODY()

public:
	/** Called when the ItemInstance is created */
	virtual void OnInstanceCreated(UItemInstance* Instance);

	/** Returns the owning item instance of the fragment. */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UItemInstance* GetOwningInstance();

protected:
	UPROPERTY(Transient)
	TObjectPtr<UItemInstance> OwningInstance = nullptr;
};
