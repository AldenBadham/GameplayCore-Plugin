// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemComponent.generated.h"

class UItemInstance;

/**
 * @class UItemComponent
 * @see UObject
 * @brief Base class for modular components that can be added to item instances.
 *
 * This class serves as the foundation for creating specialized components
 * that add specific functionalities to item instances. It allows for a flexible
 * and extensible item system.
 */
UCLASS(Abstract)
class INVENTORYSYSTEMCORE_API UItemComponent : public UObject
{
	GENERATED_BODY()

public:
	// UObject
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override { Super::GetLifetimeReplicatedProps(OutLifetimeProps); };
	// ~UObject
};
