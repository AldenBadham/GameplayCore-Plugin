// Fill out your copyright notice in the Description page of Project Settings.

#include "Definitions/Fragments/ItemFragment_Storable.h"

bool UItemFragment_Storable::CanStack() const
{
	return MaxStackCount > 1;
}

bool UItemFragment_Storable::IsUnique() const
{
	return (StorageFlags & static_cast<int32>(EItemStorageFlags::Unique)) != 0;
}
