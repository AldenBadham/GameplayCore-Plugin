// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryCache.generated.h"

class UItemDefinition;

/**
 * UInventoryCache
 * 
 * A cache system for ItemDefinitions to improve performance by reducing object creation.
 * This class manages a thread-safe cache of ItemDefinitions.
 */
UCLASS()
class INVENTORYSYSTEMCORE_API UInventoryCache : public UObject
{
	GENERATED_BODY()

	friend class UInventorySystemComponent;

public:

	UInventoryCache();
	virtual ~UInventoryCache() override;

	/**
	 * Get a cached ItemDefinitionClass.
	 * If the ItemDefinitionClass is not in the cache, it creates a new one and adds it to the cache.
	 * 
	 * @param ItemDefinitionClass The class of the ItemDefinition to retrieve or create.
	 * @return A pointer to the cached or newly created ItemDefinition.
	 */
	UItemDefinition* GetCachedDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass);
	
	/**
	 * Check if the cache contains a cached default object of the given class.
	 * 
	 * @param ItemDefinitionClass The class of the ItemDefinition.
	 * @return True if the cache contains a default object of the given class.
	 */
	bool IsCachedDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const;
	
private:

	/**
	 * Clears the cache of any ItemDefinitions that are no longer rooted.
	 * This method is called after garbage collection.
	 */
	void Clear();

	/** Map to store cached ItemDefinitions */
	UPROPERTY()
	TMap<TSubclassOf<UItemDefinition>, UItemDefinition*> CachedDefinitionMap;

	/** Critical section to ensure thread-safe access to the cache */
	FCriticalSection CacheLock;
};
