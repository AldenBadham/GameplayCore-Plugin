// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Collisions/CollisionHitPolicy.h"
#include "CollisionNotifyState.generated.h"

class AWeaponActor;
/**
 * 
 */
UCLASS()
class EQUIPMENTSYSTEMCORE_API UCollisionNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	// UAnimNotifyState
	virtual FString GetNotifyName_Implementation() const override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	// ~UAnimNotifyState

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collision")
	FName BoneName;
	
	/** Policy for handling hits during the collision window */
	UPROPERTY(EditAnywhere, Category = "Collision")
	ECollisionHitPolicy HitPolicy = ECollisionHitPolicy::OneHitPerActor;

	UPROPERTY(Transient)
	TObjectPtr<AWeaponActor> CachedWeapon = nullptr;
};
