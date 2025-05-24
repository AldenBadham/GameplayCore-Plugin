// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Data/EquipmentCache.h"

#include "Definitions/EquipmentDefinition.h"

UEquipmentCache::UEquipmentCache()
{
	// Register the cache for cleanup after garbage collection
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &UEquipmentCache::Clean);
}

UEquipmentCache::~UEquipmentCache()
{
	// Unregister from the garbage collection delegate
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
}

UEquipmentDefinition* UEquipmentCache::GetCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class)
{
	if (!IsValid(Class))
	{
		return nullptr;
	}

	// Lock the critical section to ensure thread-safe access to the cache
	FScopeLock Lock(&CacheLock);
	if (const TWeakObjectPtr<UEquipmentDefinition>* const FoundDefinition = CachedDefinitionMap.Find(Class))
	{
		if (UEquipmentDefinition* Definition = FoundDefinition->Get(); IsValid(Definition))
		{
			return Definition;
		}
	}

	// If the definition is not cached, create a new instance
	UEquipmentDefinition* NewDefinition = NewObject<UEquipmentDefinition>(this, Class);
	CachedDefinitionMap.Add(Class, NewDefinition);

	return NewDefinition;
}

bool UEquipmentCache::IsCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class)
{
	FScopeLock Lock(&CacheLock);
	const bool IsCachedDefinition = CachedDefinitionMap.Contains(Class);
	return IsCachedDefinition;
}

void UEquipmentCache::Clean()
{
	if (IsValid(this))
	{
		// Lock the critical section to ensure thread-safe access to the cache
		FScopeLock Lock(&CacheLock);
		for (auto It = CachedDefinitionMap.CreateIterator(); It; ++It)
		{
			if (It.Value() == nullptr || !It.Value().IsValid())
			{
				It.RemoveCurrent();
			}
		}
	}
}
