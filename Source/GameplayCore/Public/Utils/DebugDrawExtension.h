// // Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DebugDrawExtension.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPLAYCORE_API UDebugDrawExtension : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	static void DrawLineTraces(
		const UWorld* World,
		const FVector& Start,
		const FVector& End,
		const TArray<FHitResult>& Hits,
		const bool bDrawTraceLine,
		const FColor& LineColor,
		const float LineDuration,
		const bool bDrawHitNormal,
		const FColor& HitNormalColor,
		const float HitDuration,
		const float Thickness);

	static void DrawBoxSweep(
		const UWorld* World,
		const FVector& Start,
		const FVector& End,
		const FVector& HalfExtent,
		const FQuat& Rotation,
		const TArray<FHitResult>& Hits,
		const bool bDrawBox,
		const FColor& BoxColor,
		const float BoxDuration,
		const bool bDrawHitNormal,
		const FColor& HitNormalColor,
		const float HitDuration,
		const float Thickness);

	static void DrawSphereSweep(
		const UWorld* World,
		const FVector& Start,
		const FVector& End,
		const float Radius,
		const TArray<FHitResult>& Hits,
		const bool bDrawSphere,
		const FColor& SphereColor,
		const float SphereDuration,
		const bool bDrawHitNormal,
		const FColor& HitNormalColor,
		const float HitDuration,
		const float Thickness);

	static void DrawCapsuleSweep(
		const UWorld* World,
		const FVector& Start,
		const FVector& End,
		const float HalfHeight,
		const float Radius,
		const FQuat& Rotation,
		const TArray<FHitResult>& Hits,
		const bool bDrawCapsule,
		const FColor& CapsuleColor,
		const float CapsuleDuration,
		const bool bDrawHitNormal,
		const FColor& HitNormalColor,
		const float HitDuration,
		const float Thickness);

private:
	
	static constexpr float NormalLength = 50.f;
};
