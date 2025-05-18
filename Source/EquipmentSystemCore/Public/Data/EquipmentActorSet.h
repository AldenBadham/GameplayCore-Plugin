// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"

#include "EquipmentActorSet.generated.h"

USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FEquipmentActorSet
{
	GENERATED_BODY()

	FEquipmentActorSet()
	{
	};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (Categories = "Equipment.Slot"))
	FGameplayTag SlotTag;

	/** The class of actor to spawn and attach. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TSubclassOf<class AActor> ActorClass;

	/** The socket on the owning actor to attach the spawned actor to. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FName AttachSocket;

	/** The transform (position, rotation, scale) to apply relative to the socket. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FTransform AttachTransform;
};
