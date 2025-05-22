// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "EquipmentCache.generated.h"

class UEquipmentDefinition;

/**
 * UEquipmentCache
 *
 * A cache system for EquipmentDefinition to improve performance by reducing object creation.
 * This class manages a thread-safe cache of EquipmentDefinitions.
 */
UCLASS()
class EQUIPMENTSYSTEMCORE_API UEquipmentCache : public UObject
{
	GENERATED_BODY()

	friend class UEquipmentSystemComponent;

public:
	UEquipmentCache();
	virtual ~UEquipmentCache() override;

	/**
	 * Get a cached EquipmentDefinition default object.
	 * If the EquipmentDefinition is not in the cache, it creates a new one and adds it to the cache.
	 *
	 * @param Class The class of the EquipmentDefinition to retrieve.
	 * @return A pointer to the cached or newly created EquipmentDefinition.
	 */
	UEquipmentDefinition* GetCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class);

	/**
	 * Check if the cache contains a cached default object of the given class.
	 *
	 * @param Class The class of the EquipmentDefinition.
	 * @return True if the cache contains a default object of the given class.
	 */
	bool IsCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class);

private:
	/**
	 * Clears the cache of any EquipmentDefinitions that are no longer rooted.
	 * This method is called after garbage collection.
	 */
	void Clean();

	/** Map to store cached EquipmentDefinitions */
	UPROPERTY()
	TMap<TSubclassOf<UEquipmentDefinition>, TWeakObjectPtr<UEquipmentDefinition>> CachedDefinitionMap;

	/** Critical section to ensure thread-safe access to the cache */
	FCriticalSection CacheLock;
};