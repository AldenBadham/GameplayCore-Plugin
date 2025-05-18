// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/EquipmentCache.h"

#include "Definitions/EquipmentDefinition.h"


UEquipmentCache::UEquipmentCache()
{
	// Register the cache for cleanup after garbage collection
	FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &UEquipmentCache::Clear);
}

UEquipmentCache::~UEquipmentCache()
{
	// Unregister from the garbage collection delegate
	FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
}

UEquipmentDefinition* UEquipmentCache::GetCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class)
{
	if(!IsValid(Class))
		return nullptr;
	
	// Lock the critical section to ensure thread-safe access to the cache
	FScopeLock Lock(&CacheLock);
	
	if (UEquipmentDefinition** FoundDefinition = CachedDefinitionMap.Find(Class))
	{
		return *FoundDefinition;
	}

	// If the definition is not cached, create a new instance
	UEquipmentDefinition* NewDefinition = NewObject<UEquipmentDefinition>(this, Class);
	CachedDefinitionMap.Add(Class, NewDefinition);

	return NewDefinition;
}

bool UEquipmentCache::IsCachedDefinition(const TSubclassOf<UEquipmentDefinition>& Class) const
{
	return CachedDefinitionMap.Contains(Class);
}

void UEquipmentCache::Clear()
{
	// Lock the critical section to ensure thread-safe access to the cache
	FScopeLock Lock(&CacheLock);

	for (auto& Pair : CachedDefinitionMap)
	{
		if (Pair.Value && !Pair.Value->IsRooted())
		{
			CachedDefinitionMap.Remove(Pair.Key);
		}
	}
}
