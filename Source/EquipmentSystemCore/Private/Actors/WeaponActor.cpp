// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "Actors/WeaponActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Collisions/ShapeElementSet.h"
#include "GameplayTags/EquipmentGameplayTags.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "PhysicsEngine/SkeletalBodySetup.h"
#include "Settings/EquipmentSystemSettings.h"
#include "Utils/DebugDrawExtension.h"

// For Unreal Profiler
DECLARE_CYCLE_STAT(TEXT("CollisionTick"), STAT_CollisionTick, STATGROUP_WeaponBase);

AWeaponActor::AWeaponActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.Get())
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SetRootComponent(SkeletalMeshComponent);
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	UpdateShapeElementSets();

	AsyncTraceDelegate.BindUObject(this, &ThisClass::OnAsyncTraceComplete);
}

void AWeaponActor::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

#if !UE_BUILD_SHIPPING
	const UWorld* const World = GetWorld();
	if (!IsValid(World) || !IsValid(SkeletalMeshComponent))
	{
		return;
	}
	
	// Don't draw the debug collision if we're in GameWorld and haven't enabled collision drawing with the console variable
	if (World->IsGameWorld() && !GET_EQUIPMENT_SETTINGS_VAR(bDrawWeaponColliders))
	{
		return;
	}
	
	const float Thickness = GET_EQUIPMENT_SETTINGS_VAR(SweepTracesLineThickness);

	for (auto& [ShapeElement, BoneName, LastTransform, CollisionEnabled, ForwardHandle, BackwardHandle] : ShapeElementSets)
	{
		const FTransform BoneTransform = SkeletalMeshComponent->GetBoneTransform(BoneName, RTS_World);
		
		FTransform ShapeTransform;
		FTransform WorldTransform;
		const FVector Scale = BoneTransform.GetScale3D();
		FColor Color;
		GetCollisionColor(CollisionEnabled, Color);

		if (ShapeElement.IsType<FKSphereElem>())
		{
			FKSphereElem& Elem = ShapeElement.Get<FKSphereElem>();
			
			ShapeTransform = Elem.GetTransform();
			WorldTransform = ShapeTransform * BoneTransform;
			const float Radius = Elem.Radius * FMath::Max(Scale.X, Scale.Y);
			
			DrawDebugSphere( World, WorldTransform.GetLocation(), Radius, 12, Color, false, -1, 0, Thickness);
		}
		else if (ShapeElement.IsType<FKBoxElem>())
		{
			FKBoxElem& Elem = ShapeElement.Get<FKBoxElem>();
			ShapeTransform = Elem.GetTransform();
			WorldTransform = ShapeTransform * BoneTransform;

			FVector Extent = FVector(Elem.X * Scale.X, Elem.Y * Scale.Y, Elem.Z * Scale.Z) * 0.5f;
			
			DrawDebugBox( World, WorldTransform.GetLocation(), Extent, WorldTransform.GetRotation(), Color, false, -1, 0, Thickness);
		}
		else if (ShapeElement.IsType<FKSphylElem>())
		{
			FKSphylElem& Elem = ShapeElement.Get<FKSphylElem>();
			ShapeTransform = Elem.GetTransform();
			WorldTransform = ShapeTransform * BoneTransform;

			const float Radius = Elem.Radius * FMath::Max(Scale.X, Scale.Y);
			const float HalfHeight = (Elem.Length + Radius*2.f) * 0.5f * Scale.Z;
			
			DrawDebugCapsule( World, WorldTransform.GetLocation(), HalfHeight, Radius, WorldTransform.GetRotation(), Color, false, -1, 0, Thickness);
		}
		else if (ShapeElement.IsType<FKConvexElem>())
		{
			FKConvexElem& Elem = ShapeElement.Get<FKConvexElem>();
			ShapeTransform = Elem.GetTransform();
			WorldTransform = ShapeTransform * BoneTransform;

			TArray<FVector> Vertices = Elem.VertexData;
			for (FVector& Vertex : Vertices)
			{
				Vertex = WorldTransform.TransformPosition(Vertex);
			}
			TArray<int32> Indices = Elem.IndexData;
			
			DrawDebugMesh( World, Vertices, Indices, Color, false, -1, 0);
		}
	}
#endif
}

void AWeaponActor::OpenCollisionWindow(ECollisionHitPolicy& HitPolicy)
{
	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	CollisionWindow = FCollisionWindow(HitPolicy);
	
	// Update collision shape element sets on initial state
	UpdateShapeElementSets();
	
	World->GetTimerManager().SetTimer(
		CollisionWindow.TimerHandle,
		this,
		&AWeaponActor::OnCollisionTick,
		GET_EQUIPMENT_SETTINGS_VAR(SweepTraceTickRate),
		true);
}

void AWeaponActor::CloseCollisionWindow()
{
	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	World->GetTimerManager().ClearTimer(CollisionWindow.TimerHandle);
}

void AWeaponActor::SetPhysicsAssets(UPhysicsAsset* PhysicsAsset)
{
	const UWorld* const World = GetWorld();
	if (!IsValid(World) || !IsValid(SkeletalMeshComponent))
	{
		return;
	}

	SkeletalMeshComponent->SetPhysicsAsset(PhysicsAsset);
	UpdateShapeElementSets();
}

void AWeaponActor::OnCollisionTick()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	// Do not start a new trace if we're blocked could cause access conflict
	if (!bIsPreviousTraceComplete)
	{
		return;
	}

	
	SCOPE_CYCLE_COUNTER(STAT_CollisionTick);

	ForwardHitResults.Empty();
	
	// Prepare sweep test parameters
	FCollisionQueryParams Params;
	Params.bTraceComplex = bTraceComplex;
	Params.bReturnFaceIndex = bReturnFaceIndex;
	Params.bReturnPhysicalMaterial = bReturnPhysicalMaterial;
	Params.bIgnoreBlocks = bIgnoreBlocks;
	Params.bIgnoreTouches = bIgnoreTouches;
	Params.bSkipNarrowPhase = bSkipNarrowPhase;
	Params.bReturnPhysicalMaterial = true;

	ECollisionChannel SweepChannel = GET_EQUIPMENT_SETTINGS_VAR(SweepChannel);
	
	const TArray<AActor*> HitActors = {this, GetOwner()};
	Params.AddIgnoredActors(HitActors);

	for (auto& [ShapeElement, BoneName, LastLocation, CollisionEnabled, ForwardHandle, BackwardHandle] : ShapeElementSets)
	{
		if (!(CollisionEnabled == ECollisionEnabled::Type::QueryAndPhysics || CollisionEnabled == ECollisionEnabled::Type::QueryAndProbe || CollisionEnabled == ECollisionEnabled::Type::QueryOnly))
			continue;
		
		const FTransform BoneTransform = SkeletalMeshComponent->GetBoneTransform(BoneName, RTS_World);
		
		FTransform ShapeTransform;
		FTransform WorldTransform;
		const FVector Scale = BoneTransform.GetScale3D();
		
		FColor Color;
		GetCollisionColor(CollisionEnabled, Color);
		FCollisionShape CollisionShape;

		if (ShapeElement.IsType<FKSphereElem>())
		{
			const FKSphereElem& Elem = ShapeElement.Get<FKSphereElem>();
			
			ShapeTransform = Elem.GetTransform();
			WorldTransform = ShapeTransform * BoneTransform;
			const float Radius = Elem.Radius * FMath::Max(Scale.X, Scale.Y);
			
			CollisionShape = FCollisionShape::MakeSphere(Radius);
		}
		else if (ShapeElement.IsType<FKBoxElem>())
		{
			const FKBoxElem& Elem = ShapeElement.Get<FKBoxElem>();
			ShapeTransform = Elem.GetTransform();
			WorldTransform = ShapeTransform * BoneTransform;

			FVector Extent = FVector(Elem.X * Scale.X, Elem.Y * Scale.Y, Elem.Z * Scale.Z) * 0.5f;
			
			CollisionShape = FCollisionShape::MakeBox(Extent);
		}
		else if (ShapeElement.IsType<FKSphylElem>())
		{
			const FKSphylElem& Elem = ShapeElement.Get<FKSphylElem>();
			ShapeTransform = Elem.GetTransform();
			WorldTransform = ShapeTransform * BoneTransform;

			const float Radius = Elem.Radius * FMath::Max(Scale.X, Scale.Y);
			const float HalfHeight = (Elem.Length + Radius*2.f) * 0.5f * Scale.Z;

			CollisionShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
		}
		else if (ShapeElement.IsType<FKConvexElem>())
		{
			const FKConvexElem& Elem = ShapeElement.Get<FKConvexElem>();
			ShapeTransform = Elem.GetTransform();
			ShapeTransform.SetLocation(Elem.ElemBox.GetCenter());
			WorldTransform = ShapeTransform * BoneTransform;

			// Use the bounding box of the convex hull
			const FBox BoundingBox = Elem.ElemBox;
			const FVector Extent = BoundingBox.GetExtent() * Scale;
			
			CollisionShape = FCollisionShape::MakeBox(Extent);
		}else
		{
			continue;
		}

		FVector EndLocation = WorldTransform.GetLocation();
		
		ForwardHandle = World->AsyncSweepByChannel(
			EAsyncTraceType::Multi,
			LastLocation,
			EndLocation,
			WorldTransform.GetRotation(),
			SweepChannel,
			CollisionShape,
			Params,
			FCollisionResponseParams::DefaultResponseParam,
			&AsyncTraceDelegate);

		LastLocation = EndLocation;
	}
}

void AWeaponActor::OnAsyncTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& Data)
{
	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}
	
	auto Predicate = [&](const FShapeElementSet& Set)
	{
		return Set.ForwardTraceHandle.IsValid() && Set.ForwardTraceHandle == TraceHandle;
	};
	
	FShapeElementSet* const ShapeElementSetPtr = ShapeElementSets.FindByPredicate(Predicate);
	if (!ShapeElementSetPtr) return;		

	// Clear forward trace handle
	ShapeElementSetPtr->ForwardTraceHandle = FTraceHandle();

	for (const FHitResult& HitResult : Data.OutHits)
	{
		if (HitResult.bBlockingHit)
		{
			ForwardHitResults.Add(HitResult);
		}
	}

#if !UE_BUILD_SHIPPING
	// Gather console settings
	const bool bDrawSweepTraces = GET_EQUIPMENT_SETTINGS_VAR(bDrawSweepTraces);
	const float SweepTraceDuration = GET_EQUIPMENT_SETTINGS_VAR(SweepTracesDuration);
	const FColor SweepTraceColor = GET_EQUIPMENT_SETTINGS_VAR(SweepTracesColor);
	const bool bDrawHitNormal = GET_EQUIPMENT_SETTINGS_VAR(bDrawSweepTracesHitNormals);
	const float HitDuration = GET_EQUIPMENT_SETTINGS_VAR(SweepTracesHitNormalsDuration);
	const float LineThickness = GET_EQUIPMENT_SETTINGS_VAR(SweepTracesLineThickness);
	const FColor HitNormalColor = GET_EQUIPMENT_SETTINGS_VAR(SweepTracesHitNormalsColor);

	const FTransform BoneTransform = SkeletalMeshComponent->GetBoneTransform(ShapeElementSetPtr->BoneName, RTS_World);
	const FVector Scale = BoneTransform.GetScale3D();
	
	
	TVariant<FKSphereElem, FKBoxElem, FKSphylElem, FKConvexElem>& ShapeElement = ShapeElementSetPtr->ShapeElement;
	if (ShapeElement.IsType<FKSphereElem>())
	{
		const FKSphereElem& Elem = ShapeElement.Get<FKSphereElem>();
			
		const float Radius = Elem.Radius * FMath::Max(Scale.X, Scale.Y);
		UDebugDrawExtension::DrawSphereSweep(World, Data.Start, Data.End, Radius, Data.OutHits, bDrawSweepTraces, SweepTraceColor, SweepTraceDuration, bDrawHitNormal, HitNormalColor, HitDuration, LineThickness);
	}
	else if (ShapeElement.IsType<FKBoxElem>())
	{
		const FKBoxElem& Elem = ShapeElement.Get<FKBoxElem>();
		const FTransform ShapeTransform = Elem.GetTransform();
		const FTransform WorldTransform = ShapeTransform * BoneTransform;

		FVector Extent = FVector(Elem.X * Scale.X, Elem.Y * Scale.Y, Elem.Z * Scale.Z) * 0.5f;
			
		UDebugDrawExtension::DrawBoxSweep(World, Data.Start, Data.End, Extent, WorldTransform.GetRotation(), Data.OutHits, bDrawSweepTraces, SweepTraceColor, SweepTraceDuration, bDrawHitNormal, HitNormalColor, HitDuration, LineThickness);
	}
	else if (ShapeElement.IsType<FKSphylElem>())
	{
		const FKSphylElem& Elem = ShapeElement.Get<FKSphylElem>();
		const FTransform ShapeTransform = Elem.GetTransform();
		const FTransform WorldTransform = ShapeTransform * BoneTransform;

		const float Radius = Elem.Radius * FMath::Max(Scale.X, Scale.Y);
		const float HalfHeight = (Elem.Length + Radius*2.f) * 0.5f * Scale.Z;

		UDebugDrawExtension::DrawCapsuleSweep(World, Data.Start, Data.End, HalfHeight, Radius, WorldTransform.GetRotation(), Data.OutHits, bDrawSweepTraces, SweepTraceColor, SweepTraceDuration, bDrawHitNormal, HitNormalColor, HitDuration, LineThickness);
	}
	else if (ShapeElement.IsType<FKConvexElem>())
	{
		const FKConvexElem& Elem = ShapeElement.Get<FKConvexElem>();
		const FTransform ShapeTransform = Elem.GetTransform();
		const FTransform WorldTransform = ShapeTransform * BoneTransform;

		// Use the bounding box of the convex hull
		const FBox BoundingBox = Elem.ElemBox;
		const FVector Extent = BoundingBox.GetExtent() * Scale;
			
		UDebugDrawExtension::DrawBoxSweep(World, Data.Start, Data.End, Extent, WorldTransform.GetRotation(), Data.OutHits, bDrawSweepTraces, SweepTraceColor, SweepTraceDuration, bDrawHitNormal, HitNormalColor, HitDuration, LineThickness);
	}
#endif

	// Check if all the async traces are finished
	bool AreAllTracesFinished = true;
	for (FShapeElementSet& ElementSet : ShapeElementSets)
	{
		if (ElementSet.ForwardTraceHandle.IsValid())
		{
			AreAllTracesFinished = false;
			break;
		}
	}

	if (AreAllTracesFinished) ProcessHitResults();

	bIsPreviousTraceComplete = AreAllTracesFinished;
}

void AWeaponActor::ProcessHitResults()
{
	for (const FHitResult& HitResult : ForwardHitResults)
	{
		ComputeCollisionHit(HitResult);
	}
}

void AWeaponActor::ComputeCollisionHit(const FHitResult& SweepResult)
{
	OnSweepCollisionHit.Broadcast(SweepResult);
	K2_OnSweepCollisionHit(SweepResult);

	// Do not do anything if we already touched an actor in OneHitOnly
	if (CollisionWindow.Policy == ECollisionHitPolicy::OneHitOnly && CollisionWindow.HitActors.Num() > 0)
	{
		return;		
	}
	
	const APawn* const InstigatorPawn = GetInstigator();
	AActor* const TargetActor = SweepResult.GetActor();	
	if (IsValid(TargetActor) && IsValid(InstigatorPawn) && InstigatorPawn != TargetActor)
	{
		// Do not send event if we already hit this actor in OneHitPerActor or OneHitPerSuccessiveActor
		if (CollisionWindow.Policy > ECollisionHitPolicy::OneHitOnly
			&& CollisionWindow.HitActors.Contains(TargetActor))
		{
			return;
		}

		// Send gameplay event to instigator ability system component
		if (InstigatorPawn->Implements<UAbilitySystemInterface>())
		{
			if (const IAbilitySystemInterface* Implementer = Cast<IAbilitySystemInterface>(InstigatorPawn))
			{
				if (UAbilitySystemComponent* InstigatorASC = Implementer->GetAbilitySystemComponent(); IsValid(InstigatorASC))
				{
					FScopedPredictionWindow PredictionWindow(InstigatorASC, true);

					FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
					TargetData->HitResult = SweepResult;

					FGameplayEventData Payload;
					Payload.Instigator = InstigatorPawn;
					Payload.Target = TargetActor;
					Payload.EventTag = EquipmentSystemGameplayTags::TAG_Equipment_Event_CollisionHit;
					Payload.InstigatorTags = InstigatorASC->GetOwnedGameplayTags();
					Payload.OptionalObject = this;
					Payload.TargetData = FGameplayAbilityTargetDataHandle(TargetData);

					if (TargetActor->Implements<UAbilitySystemInterface>())
					{
						if (UAbilitySystemComponent* TargetASC = Implementer->GetAbilitySystemComponent(); IsValid(TargetASC))
						{
							Payload.TargetTags = TargetASC->GetOwnedGameplayTags();
						}
					}

					InstigatorASC->HandleGameplayEvent(Payload.EventTag, &Payload);
				}
			}			
		}

		// Manage hit history depending on the window policy
		if (CollisionWindow.Policy == ECollisionHitPolicy::OneHitPerSuccessiveActor)
		{
			CollisionWindow.HitActors.Empty(1);
		}
		CollisionWindow.HitActors.Emplace(TargetActor);
	}
}

void AWeaponActor::UpdateShapeElementSets()
{
	UPhysicsAsset* PhysicsAsset = SkeletalMeshComponent->GetPhysicsAsset();
	if (!IsValid(PhysicsAsset))
	{
		ShapeElementSets.Empty();
		CachedPhysicsAsset = nullptr;
		return;
	}

	ShapeElementSets.Empty(PhysicsAsset->SkeletalBodySetups.Num());

	for (USkeletalBodySetup* const BodySetup : PhysicsAsset->SkeletalBodySetups)
	{
		if (!IsValid(BodySetup)) continue;

		FName BoneName = BodySetup->BoneName;
		const FTransform BoneTransform = SkeletalMeshComponent->GetBoneTransform(BoneName, RTS_World);
		const FVector Scale = BoneTransform.GetScale3D();

		for (FKSphereElem& Elem : BodySetup->AggGeom.SphereElems)
		{
			FShapeElementSet Set;
			Set.ShapeElement.Set<FKSphereElem>(Elem);
			
			Set.BoneName = BoneName;
			Set.CollisionEnabled = Elem.GetCollisionEnabled();
			Set.LastLocation = (Elem.GetTransform() * BoneTransform).GetLocation();
			
			ShapeElementSets.Emplace(Set);
		}
		for (FKBoxElem& Elem : BodySetup->AggGeom.BoxElems)
		{
			FShapeElementSet Set;
			Set.ShapeElement.Set<FKBoxElem>(Elem);
			Set.BoneName = BoneName;
			Set.CollisionEnabled = Elem.GetCollisionEnabled();
			Set.LastLocation = (Elem.GetTransform() * BoneTransform).GetLocation();
			
			ShapeElementSets.Emplace(Set);
		}
		for (FKSphylElem& Elem : BodySetup->AggGeom.SphylElems)
		{
			FShapeElementSet Set;
			Set.ShapeElement.Set<FKSphylElem>(Elem);
			Set.BoneName = BoneName;
			Set.CollisionEnabled = Elem.GetCollisionEnabled();
			Set.LastLocation = (Elem.GetTransform() * BoneTransform).GetLocation();
			
			ShapeElementSets.Emplace(Set);
		}
		for (FKConvexElem& Elem : BodySetup->AggGeom.ConvexElems)
		{
			FShapeElementSet Set;
			Set.ShapeElement.Set<FKConvexElem>(Elem);
			Set.BoneName = BoneName;
			Set.CollisionEnabled = Elem.GetCollisionEnabled();
			
			FTransform ShapeTransform = Elem.GetTransform();
			ShapeTransform.SetLocation(Elem.ElemBox.GetCenter());
			Set.LastLocation = (BoneTransform * ShapeTransform).GetLocation();
			
			ShapeElementSets.Emplace(Set);
		}
	}

	CachedPhysicsAsset = PhysicsAsset;
}

void AWeaponActor::GetCollisionColor(const ECollisionEnabled::Type CollisionType, FColor& Color)
{
	switch (CollisionType)
	{
	case ECollisionEnabled::NoCollision:
		Color = FColor::Red;
		break;
	case ECollisionEnabled::QueryOnly:
		Color = FColor::Blue;
		break;
	case ECollisionEnabled::PhysicsOnly:
		Color = FColor::Yellow;
		break;
	case ECollisionEnabled::QueryAndPhysics:
		Color = FColor::Green;
		break;
	case ECollisionEnabled::ProbeOnly:
		Color = FColor::Magenta;
		break;
	case ECollisionEnabled::QueryAndProbe:
		Color = FColor::Purple;
		break;
	default:
		Color = FColor::White;
		break;
	}
}
