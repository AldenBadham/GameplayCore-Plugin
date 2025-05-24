// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/InventoryCache.h"

#include "Definitions/ItemDefinition.h"

UInventoryCache::UInventoryCache()
{
	// Register the cache for cleanup after garbage collection
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &UInventoryCache::Clear);
}

UInventoryCache::~UInventoryCache()
{
	// Unregister from the garbage collection delegate
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
}

UItemDefinition* UInventoryCache::GetCachedDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass)
{
	if (!IsValid(ItemDefinitionClass))
	{
		return nullptr;
	}

	// Lock the critical section to ensure thread-safe access to the cache
	FScopeLock Lock(&CacheLock);

	if (const TWeakObjectPtr<UItemDefinition>* FoundDefinition = CachedDefinitionMap.Find(ItemDefinitionClass))
	{
		if (UItemDefinition* Definition = FoundDefinition->Get(); IsValid(Definition))
		{
			return Definition;
		}
	}

	// If the definition is not cached, create a new instance
	UItemDefinition* NewDefinition = NewObject<UItemDefinition>(this, ItemDefinitionClass);
	CachedDefinitionMap.Add(ItemDefinitionClass, NewDefinition);

	return NewDefinition;
}

bool UInventoryCache::IsCachedDefinition(const TSubclassOf<UItemDefinition>& ItemDefinitionClass) const
{
	return CachedDefinitionMap.Contains(ItemDefinitionClass);
}

void UInventoryCache::Clear()
{
	// Lock the critical section to ensure thread-safe access to the cache
	FScopeLock Lock(&CacheLock);

	for (auto& Pair : CachedDefinitionMap)
	{
		if (Pair.Value != nullptr && !Pair.Value->IsRooted())
		{
			CachedDefinitionMap.Remove(Pair.Key);
		}
	}
}
