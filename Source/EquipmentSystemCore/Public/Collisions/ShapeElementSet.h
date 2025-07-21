// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/ConvexElem.h"
#include "PhysicsEngine/ShapeElem.h"
#include "PhysicsEngine/SphereElem.h"
#include "PhysicsEngine/SphylElem.h"

#include "ShapeElementSet.generated.h"


/**
 * FAggregateGeomSet
 */
USTRUCT(BlueprintType)
struct EQUIPMENTSYSTEMCORE_API FShapeElementSet
{
	GENERATED_BODY();

public:

	FShapeElementSet() = default;
	FShapeElementSet(const TVariant<FKSphereElem, FKBoxElem, FKSphylElem, FKConvexElem>& InShapeElement, const FName& InBoneName, const FVector& InLocation = FVector::ZeroVector)
		: ShapeElement(InShapeElement), BoneName(InBoneName), LastLocation(InLocation){}

public:

	TVariant<FKSphereElem, FKBoxElem, FKSphylElem, FKConvexElem> ShapeElement;
	
	UPROPERTY()
	FName BoneName = NAME_None;
	
	UPROPERTY()
	FVector LastLocation = FVector::ZeroVector;

	UPROPERTY()
	TEnumAsByte<ECollisionEnabled::Type> CollisionEnabled = ECollisionEnabled::NoCollision;
	
	// Handle for asynchronous tracing
	FTraceHandle ForwardTraceHandle = FTraceHandle();
	FTraceHandle ReverseTraceHandle = FTraceHandle();
};