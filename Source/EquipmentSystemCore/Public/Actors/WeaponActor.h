// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentActor.h"
#include "Collisions/CollisionHitPolicy.h"
#include "Collisions/CollisionWindow.h"
#include "WeaponActor.generated.h"

class USkeletalMeshComponent;
struct FShapeElementSet;

// For Unreal Profiler
DECLARE_STATS_GROUP(TEXT("AWeaponActor"), STATGROUP_WeaponBase, STATCAT_Advanced);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FOnSweepCollisionHit, const FHitResult&, SweepResult);

/**
 * AWeaponActor
 *
 * Base class for weapons in the game, inheriting from AEquipmentActor.
 * Manages collision profiles, gameplay tags, collision windows, and handles overlap events.
 */
UCLASS(BlueprintType, Blueprintable)
class EQUIPMENTSYSTEMCORE_API AWeaponActor : public AEquipmentActor
{
	GENERATED_BODY()

public:

	AWeaponActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// UObject
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// ~UObject

	/** Delegate called when the weapon collide on a sweep test. */
	UPROPERTY(BlueprintAssignable, Category = "Collision")
	FOnSweepCollisionHit OnSweepCollisionHit;
	
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void OpenCollisionWindow(ECollisionHitPolicy& HitPolicy);
	UFUNCTION(BlueprintCallable, Category = "Collision")
	void CloseCollisionWindow();

	/**
	 * Blueprint event called when a collision component begins overlapping with another component.
	 * @param SweepResult Result of the sweep if it occurred.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Collision", DisplayName = "OnSweepCollisionHit")
	void K2_OnSweepCollisionHit(const FHitResult& SweepResult);


	UFUNCTION(BlueprintCallable, Category = "Collision")
	void SetPhysicsAssets(UPhysicsAsset* PhysicsAsset);
	
protected:

	UFUNCTION()
	virtual void OnCollisionTick();
	
	void OnAsyncTraceComplete(const FTraceHandle& TraceHandle, FTraceDatum& Data);
	
	void ProcessHitResults();
	void ComputeCollisionHit(const FHitResult& SweepResult);

	void UpdateShapeElementSets();

	static void GetCollisionColor(const ECollisionEnabled::Type CollisionType, FColor& Color);

protected:
	
	/** Skeletal mesh component of the weapon */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

	
	/** Whether we should trace against complex collision */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Sweep Collision", AdvancedDisplay, meta = (DisplayName = "Trace Complex", AllowPrivateAccess))
	bool bTraceComplex = false;
	
	/** Whether we want to return the triangle face index for complex static mesh traces */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Sweep Collision", AdvancedDisplay, meta = (DisplayName = "Return Face Index", AllowPrivateAccess))
	bool bReturnFaceIndex = false;

	/** Whether we want to include the physical material in the results. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Sweep Collision", AdvancedDisplay, meta = (DisplayName = "Return Physical Material", AllowPrivateAccess))
	bool bReturnPhysicalMaterial = false;

	/** Whether to ignore blocking results. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Sweep Collision", AdvancedDisplay, meta = (DisplayName = "Ignore Blocks", AllowPrivateAccess))
	bool bIgnoreBlocks = false;

	/** Whether to ignore touch/overlap results. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Sweep Collision", AdvancedDisplay, meta = (DisplayName = "Ignore Touches", AllowPrivateAccess))
	bool bIgnoreTouches = false;

	/** Whether to skip narrow phase checks (only for overlaps). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Collision|Sweep Collision", AdvancedDisplay, meta = (DisplayName = "Skip Narrow Phase", AllowPrivateAccess))
	bool bSkipNarrowPhase = false;

	
	UPROPERTY()
	FCollisionWindow CollisionWindow;

	UPROPERTY()
	TArray<FShapeElementSet> ShapeElementSets;

	UPROPERTY()
	TObjectPtr<UPhysicsAsset> CachedPhysicsAsset = nullptr;

	UPROPERTY()
	bool bIsPreviousTraceComplete = true;

	FTraceDelegate AsyncTraceDelegate;
	TArray<FHitResult> ForwardHitResults;
};
