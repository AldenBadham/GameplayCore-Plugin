// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "EquipmentFragment.generated.h"

class UEquipmentInstance;

/**
 * @class UEquipmentFragment
 * @see UEquipmentDefinition
 * @brief Represents a fragment of an equipment definition, allow definition extension.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType, Abstract)
class EQUIPMENTSYSTEMCORE_API UEquipmentFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UEquipmentInstance* Instance) const
	{
	}
};
