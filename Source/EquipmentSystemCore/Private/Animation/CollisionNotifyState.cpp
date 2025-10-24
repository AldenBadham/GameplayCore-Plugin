// Licensed under the MIT License. See the LICENSE file in the project root for full license information.


#include "Animation/CollisionNotifyState.h"

#include "Actors/WeaponActor.h"
#include "Components/EquipmentSystemComponent.h"
#include "Interfaces/EquipmentSystemInterface.h"

FString UCollisionNotifyState::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Collision [%s]"), *BoneName.ToString());
}

void UCollisionNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!IsValid(MeshComp))
	{
		UE_LOG(LogAnimation, Warning, TEXT("Attempt to begin a notification with an invalid component or collision set."));
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	
	if (!IsValid(Owner) || !Owner->Implements<UEquipmentSystemInterface>())
	{
		return;
	}

	if (const IEquipmentSystemInterface* Implementer = Cast<IEquipmentSystemInterface>(Owner))
	{
		if (const UEquipmentSystemComponent* const EquipmentSystemComponent = Implementer->GetEquipmentSystemComponent(); IsValid(EquipmentSystemComponent))
		{
			if (AWeaponActor* Weapon = EquipmentSystemComponent->FindWeaponActorAttachedToBone(BoneName); IsValid(Weapon))
			{
				Weapon->OpenCollisionWindow(HitPolicy);
				CachedWeapon = Weapon;
			}			
		}
	}
}

void UCollisionNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		UE_LOG(LogAnimation, Warning, TEXT("Attempt to end a notification with an invalid component or collision set."));
		return;
	}

	if (IsValid(CachedWeapon))
	{
		CachedWeapon->CloseCollisionWindow();
	}
}
