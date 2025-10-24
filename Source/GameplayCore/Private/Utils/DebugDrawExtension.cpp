// Licensed under the MIT License. See the LICENSE file in the project root for full license information.


#include "Utils/DebugDrawExtension.h"

// Source code by PlatoManchi
// https://github.com/PlatoManchi

void UDebugDrawExtension::DrawLineTraces(
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
	const float Thickness)
{
#if !UE_BUILD_SHIPPING
	if (bDrawTraceLine)
	{
		DrawDebugPoint(World, Start, Thickness + 10.0f, LineColor, false, LineDuration, 0);
		DrawDebugPoint(World, End, Thickness + 10.0f, LineColor, false, LineDuration, 0);
		DrawDebugLine(World, Start, End, LineColor, false, LineDuration, 0, Thickness);
	}

	if (bDrawHitNormal)
	{
		for (const FHitResult& Hit : Hits)
		{
			const auto HitActor = Hit.GetActor();
			FVector NormalStart = Hit.ImpactPoint;
			FVector NormalEnd = NormalStart + (Hit.ImpactNormal * NormalLength);

			DrawDebugDirectionalArrow(
				World,
				NormalStart,
				NormalEnd,
				5.f,
				HitNormalColor,
				false,
				HitDuration,
				0,
				Thickness * 2.0f);
		}
	}
#endif
}

void UDebugDrawExtension::DrawBoxSweep(
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
	const float Thickness)
{
#if !UE_BUILD_SHIPPING
	FBox BoxStart(Start - HalfExtent, Start + HalfExtent);
	FBox BoxEnd(End - HalfExtent, End + HalfExtent);

	if (bDrawBox)
	{
		FColor Color = BoxColor;

		DrawDebugBox(
			World,
			BoxStart.GetCenter(),
			BoxStart.GetExtent(),
			Rotation,
			Color,
			false,
			BoxDuration,
			0,
			Thickness);
		DrawDebugBox(World, BoxEnd.GetCenter(), BoxEnd.GetExtent(), Rotation, Color, false, BoxDuration, 0, Thickness);

		// Connect the vertices of start box with end box
		// since start_box and end_box are already translated applying rotation will lead to rotating translated box
		// around origin.
		// so construct each vertex of the box as if the center of box is at origin and rotate the point
		// and then translate it back to start and end positions.
		const FTransform BoxTransform(Rotation);
		// (-1, -1, 1)
		FVector TransformPoint = BoxTransform.TransformPosition(FVector(-HalfExtent.X, -HalfExtent.Y, HalfExtent.Z));
		DrawDebugLine(World, TransformPoint + Start, TransformPoint + End, Color, false, BoxDuration, 0, Thickness);
		// (1, -1, 1)
		TransformPoint = BoxTransform.TransformPosition(FVector(HalfExtent.X, -HalfExtent.Y, HalfExtent.Z));
		DrawDebugLine(World, TransformPoint + Start, TransformPoint + End, Color, false, BoxDuration, 0, Thickness);
		// (1, 1, 1)
		TransformPoint = BoxTransform.TransformPosition(FVector(HalfExtent.X, HalfExtent.Y, HalfExtent.Z));
		DrawDebugLine(World, TransformPoint + Start, TransformPoint + End, Color, false, BoxDuration, 0, Thickness);
		// (-1, 1, 1)
		TransformPoint = BoxTransform.TransformPosition(FVector(-HalfExtent.X, HalfExtent.Y, HalfExtent.Z));
		DrawDebugLine(World, TransformPoint + Start, TransformPoint + End, Color, false, BoxDuration, 0, Thickness);
		//(-1, -1, -1)
		TransformPoint = BoxTransform.TransformPosition(FVector(-HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z));
		DrawDebugLine(World, TransformPoint + Start, TransformPoint + End, Color, false, BoxDuration, 0, Thickness);
		// (1, -1, -1)
		TransformPoint = BoxTransform.TransformPosition(FVector(HalfExtent.X, -HalfExtent.Y, -HalfExtent.Z));
		DrawDebugLine(World, TransformPoint + Start, TransformPoint + End, Color, false, BoxDuration, 0, Thickness);
		//(1, 1, -1)
		TransformPoint = BoxTransform.TransformPosition(FVector(HalfExtent.X, HalfExtent.Y, -HalfExtent.Z));
		DrawDebugLine(World, TransformPoint + Start, TransformPoint + End, Color, false, BoxDuration, 0, Thickness);
		// (-1, 1, -1)
		TransformPoint = BoxTransform.TransformPosition(FVector(-HalfExtent.X, HalfExtent.Y, -HalfExtent.Z));
		DrawDebugLine(World, TransformPoint + Start, TransformPoint + End, Color, false, BoxDuration, 0, Thickness);
	}

	if (bDrawHitNormal)
	{
		for (const FHitResult& Hit : Hits)
		{
			const auto HitActor = Hit.GetActor();
			FVector NormalStart = Hit.ImpactPoint;
			FVector NormalEnd = NormalStart + (Hit.ImpactNormal * NormalLength);

			DrawDebugDirectionalArrow(
				World,
				NormalStart,
				NormalEnd,
				5.f,
				HitNormalColor,
				false,
				HitDuration,
				0,
				Thickness * 2.0f);
		}
	}

#endif
}

void UDebugDrawExtension::DrawSphereSweep(
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
	const float Thickness)
{
#if !UE_BUILD_SHIPPING
	if (bDrawSphere)
	{
		const FColor Color = SphereColor;
		DrawDebugSphere(
			World,
			Start,
			Radius,
			FMath::Max(Radius * 0.25f, 2.f),
			Color,
			false,
			SphereDuration,
			0,
			Thickness);
		DrawDebugSphere(
			World,
			End,
			Radius,
			FMath::Max(Radius * 0.25f, 2.f),
			Color,
			false,
			SphereDuration,
			0,
			Thickness);
		DrawDebugLine(
			World,
			Start + FVector(0, 0, Radius),
			End + FVector(0, 0, Radius),
			Color,
			false,
			SphereDuration,
			0,
			Thickness);
		DrawDebugLine(
			World,
			Start - FVector(0, 0, Radius),
			End - FVector(0, 0, Radius),
			Color,
			false,
			SphereDuration,
			0,
			Thickness);
	}

	if (bDrawHitNormal)
	{
		for (const FHitResult& Hit : Hits)
		{
			const auto HitActor = Hit.GetActor();
			FVector NormalStart = Hit.ImpactPoint;
			FVector NormalEnd = NormalStart + (Hit.ImpactNormal * NormalLength);

			DrawDebugDirectionalArrow(
				World,
				NormalStart,
				NormalEnd,
				5.f,
				HitNormalColor,
				false,
				HitDuration,
				0,
				Thickness * 2.0f);
		}
	}

#endif
}

void UDebugDrawExtension::DrawCapsuleSweep(
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
	const float Thickness)
{
#if !UE_BUILD_SHIPPING
	if (bDrawCapsule)
	{
		const FColor Color = CapsuleColor;
		DrawDebugLine(World, Start, End, Color, false, CapsuleDuration, 0, Thickness);
		DrawDebugCapsule(World, Start, HalfHeight, Radius, Rotation, Color, false, CapsuleDuration, 0, Thickness);
		DrawDebugCapsule(World, End, HalfHeight, Radius, Rotation, Color, false, CapsuleDuration, 0, Thickness);

		const FVector CenterToEndTip = Rotation.RotateVector(FVector(0, 0, HalfHeight));

		DrawDebugLine(World, Start + CenterToEndTip, End + CenterToEndTip, Color, false, CapsuleDuration, 0, Thickness);
		DrawDebugLine(World, Start - CenterToEndTip, End - CenterToEndTip, Color, false, CapsuleDuration, 0, Thickness);

		FVector Direction = (End - Start);
		Direction.Normalize();
		FVector Right = Direction ^ FVector::ZAxisVector;
		Right *= Radius;

		DrawDebugLine(World, Start - Right, End - Right, Color, false, CapsuleDuration, 0, Thickness);
		DrawDebugLine(World, Start + Right, End + Right, Color, false, CapsuleDuration, 0, Thickness);
	}

	if (bDrawHitNormal)
	{
		for (const FHitResult& Hit : Hits)
		{
			if (Hit.bBlockingHit)
			{
				const auto HitActor = Hit.GetActor();
				FVector NormalStart = Hit.ImpactPoint;
				FVector NormalEnd = NormalStart + (Hit.ImpactNormal * NormalLength);

				DrawDebugDirectionalArrow(
					World,
					NormalStart,
					NormalEnd,
					5.f,
					HitNormalColor,
					false,
					HitDuration,
					0,
					Thickness * 2.0f);
			}
		}
	}
#endif
}
