// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/AbilitySetHandles.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "EquipmentEntry.generated.h"


class UEquipmentDefinition;
class UEquipmentInstance;
/**
 * @class FEquipmentEntry
 *
 * @see FFastArraySerializerItem
 * @brief Entry of a single equipped equipment
 */
USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	friend class UEquipmentSystemComponent;
	friend struct FEquipmentList;

	FEquipmentEntry()
	{
	}

	FString GetDebugString() const;

protected:
	UPROPERTY(SaveGame)
	TObjectPtr<UEquipmentInstance> Instance = nullptr;

	UPROPERTY(SaveGame)
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	// Authority-only list of granted handles for ability sets
	UPROPERTY(NotReplicated)
	FAbilitySetHandles Handles;
};
