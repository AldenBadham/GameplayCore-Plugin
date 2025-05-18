// Fill out your copyright notice in the Description page of Project Settings.


#include "Instances/EquipmentInstance.h"

#include "GameFramework/Character.h"
#include "Log/EquipmentSystemLog.h"
#include "Net/UnrealNetwork.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"
#endif

UEquipmentInstance::UEquipmentInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.Get())
{
}

UWorld* UEquipmentInstance::GetWorld() const
{
	if (const APawn* OwningPawn = GetPawn(); IsValid(OwningPawn))
	{
		return OwningPawn->GetWorld();
	}
	return nullptr;
}

void UEquipmentInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
	DOREPLIFETIME(ThisClass, Components);
}

APawn* UEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UEquipmentInstance::GetTypedPawn(const TSubclassOf<APawn> Type) const
{
	if (UClass* PawnClass = Type->GetClass())
	{
		if (GetOuter()->IsA(PawnClass))
		{
			return Cast<APawn>(GetOuter());
		}
	}
	return nullptr;
}

UEquipmentComponent* UEquipmentInstance::AddComponent(const TSubclassOf<UEquipmentComponent> ComponentClass)
{
	if (const auto Component = NewObject<UEquipmentComponent>(ComponentClass); IsValid(Component))
	{
		Components.Add(Component);
		return Component;
	}
	return nullptr;
}

const UEquipmentComponent* UEquipmentInstance::FindComponentByClass(const TSubclassOf<UEquipmentComponent> ComponentClass) const
{
	if (IsValid(ComponentClass))
	{
		for (const UEquipmentComponent* Component : Components)
		{
			if (IsValid(Component) && Component->IsA(ComponentClass))
			{
				return Component;
			}
		}
	}

	return nullptr;
}

UObject* UEquipmentInstance::GetInstigator() const
{
	return Instigator;
}

void UEquipmentInstance::SetInstigator(UObject* InInstigator)
{
	Instigator = InInstigator;
}

UItemInstance* UEquipmentInstance::GetSourceItem() const
{
	return SourceItem;
}

TSubclassOf<UEquipmentDefinition> UEquipmentInstance::GetDefinitionClass() const
{
	return DefinitionClass;
}

UEquipmentDefinition* UEquipmentInstance::GetDefinition() const
{
	return Definition.Get();
}

void UEquipmentInstance::SetSourceItem(UItemInstance* InSourceObject)
{
	SourceItem = InSourceObject;
}

void UEquipmentInstance::SetDefinition(UEquipmentDefinition* InDefinition)
{
	Definition = InDefinition;
	DefinitionClass = InDefinition->GetClass();
}

void UEquipmentInstance::SpawnActors(const TArray<FEquipmentActorSet>& ActorsToSpawn)
{
	UWorld* World = GetWorld();
	verifyf(IsValid(World), TEXT("Invalid world to spawn actors for %s"), *GetName());

	if (APawn* OwningPawn = GetPawn(); IsValid(OwningPawn))
	{
		// Use root scene component as attachment target by default
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (const ACharacter* Character = Cast<ACharacter>(OwningPawn))
		{
			AttachTarget = Character->GetMesh();
		}

		for (const auto& [SlotTag, ActorClass, AttachSocket, AttachTransform] : ActorsToSpawn)
		{
			if (!IsValid(ActorClass))
			{
				UE_LOG(LogEquipmentSystem, Warning, TEXT("Tried to spawn equipment actors with an invalid actor class for %s!"), *GetName());
				continue;
			}

			if (const auto NewActor = World->SpawnActorDeferred<AActor>(ActorClass, FTransform::Identity, OwningPawn); IsValid(NewActor))
			{
				NewActor->SetInstigator(OwningPawn);
				NewActor->FinishSpawning(FTransform::Identity, true);

				// Set relative transform before attaching
				NewActor->SetActorRelativeTransform(AttachTransform);
				NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);

				SpawnedActors.Add(NewActor);
			}
		}
		return;
	}

	UE_LOG(LogEquipmentSystem, Error, TEXT("Tried to spawn equipment actors with an invalid pawn !"));
}

void UEquipmentInstance::DestroyActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
}

void UEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}

void UEquipmentInstance::OnSpawned()
{
}

void UEquipmentInstance::OnRep_Instigator()
{
}
