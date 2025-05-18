// Fill out your copyright notice in the Description page of Project Settings.

#include "Definitions/Fragments/ItemFragment.h"

void UItemFragment::OnInstanceCreated(UItemInstance* Instance)
{
	check(Instance);
	OwningInstance = Instance;
}

UItemInstance* UItemFragment::GetOwningInstance()
{
	return OwningInstance;
}
