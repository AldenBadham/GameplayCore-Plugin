// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "CollisionHitPolicy.h"

#include "CollisionWindow.generated.h"


/**
 * FCollisionWindow
 *
 * This structure represents a collision window triggered by the weapon collision anim notify
 */
USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FCollisionWindow
{
	GENERATED_BODY();

public:

	/**
	 * Constructor initializing the collision window with provided parameters.
	 *
	 * @param InPolicy The hit policy determining how collisions are processed.
	 */
	explicit FCollisionWindow( const ECollisionHitPolicy InPolicy = ECollisionHitPolicy::OneHitPerActor) : Policy(InPolicy)
	{
		// Reserve space for hit actors based on policy
		if (Policy == ECollisionHitPolicy::OneHitOnly || Policy == ECollisionHitPolicy::OneHitPerSuccessiveActor)
		{
			HitActors.Reserve(1);
		}
	}

public:

	/** Hit policy determining how collisions are processed */
	UPROPERTY(EditAnywhere)
	ECollisionHitPolicy Policy = ECollisionHitPolicy::OneHitPerActor;
	
	/** Array of actors hit during collision detection */
	UPROPERTY(EditAnywhere)
	TArray<AActor*> HitActors = {};
	
	UPROPERTY()
	FTimerHandle TimerHandle = FTimerHandle();
};