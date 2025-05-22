// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryCache.generated.h"

class UItemDefinition;

/**
 * @class UInventoryCache
 * @see UObject
 * @brief A thread-safe cache system for ItemDefinitions
 * @details This class manages a cache of ItemDefinitions to improve performance by reducing object creation 
 * and providing thread-safe access to cached objects.
 */
UCLASS(HideDropdown, Hidden)
class INVENTORYSYSTEMCORE_API UInventoryCache : public UObject
{
	GENERATED_BODY()

	friend class UInventorySystemComponent;

public:
	UInventoryCache();
	virtual ~UInventoryCache() override;

	/**
	 * Gets or creates a cached ItemDefinition instance in a thread-safe manner
	 * If the ItemDefinition is not in the cache, creates a new one and adds it to the cache
	 *
	 * @param ItemDefinitionClass The class of the ItemDefinition to retrieve or create
	 * @return A pointer to the cached or newly created ItemDefinition instance
	 * @see UItemDefinition
	 */
	UItemDefinition* GetCachedDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass);

	/**
	 * Checks if an ItemDefinition of the specified class exists in the cache
	 *
	 * @param ItemDefinitionClass The class of the ItemDefinition to check
	 * @return True if the cache contains a valid instance of the specified class, false otherwise
	 * @see UItemDefinition
	 */
	bool IsCachedDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const;

private:
	/**
	 * Clears the cache of any ItemDefinitions that are no longer rooted
	 * This method is called after garbage collection and is thread-safe
	 * @see CacheLock
	 */
	void Clear();

	/**
	 * Storage for cached ItemDefinition instances
	 * Maps ItemDefinition classes to their corresponding cached instances
	 */
	UPROPERTY()
	TMap<TSubclassOf<UItemDefinition>, TWeakObjectPtr<UItemDefinition>> CachedDefinitionMap;

	/**
	 * Synchronization primitive for thread-safe access to the CachedDefinitionMap
	 * Used to prevent concurrent modifications to the cache
	 */
	FCriticalSection CacheLock;
};