// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

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
 * @brief Entry of single equipped equipment
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

	~FEquipmentEntry()
	{
	};

	// FFastArraySerializer
	// Functions not virtual in FFAstArraySerializer because called by template
	void PreReplicatedRemove(const FEquipmentList& InArraySerializer) const;
	void PostReplicatedAdd(const FEquipmentList& InArraySerializer) const;
	void PostReplicatedChange(const FEquipmentList& InArraySerializer) const;
	FString GetDebugString() const;
	// ~FFastArraySerializer

protected:
	UPROPERTY(SaveGame)
	TObjectPtr<UEquipmentInstance> Instance = nullptr;

	UPROPERTY(SaveGame)
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	// Authority-only list of granted handles for ability sets
	UPROPERTY(NotReplicated)
	FAbilitySetHandles Handles;

	UPROPERTY(NotReplicated)
	TWeakObjectPtr<UEquipmentInstance> LastInstance;
};
