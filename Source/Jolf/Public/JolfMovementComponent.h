// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/MovementComponent.h"
#include "JolfMovementComponent.generated.h"

UCLASS()
class UJolfMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:

	UJolfMovementComponent(const FObjectInitializer& ObjectInitializer);

public: // Functions

	void SimulateProjectile(float InDeltaTime);

	/** Used when correcting a client misprediction. Rewinds idle time as well. */
	void Resimulate(int32 NumFrames);

	/** Get number of seconds since last movement. Updated by simulation tick. */
	float GetIdleTime() const { return IdleTime; }

	/** Mark non-idle. Important when setting velocity to avoid accidentally treating as idle before simulation ticks. */
	void ResetIdleTime() { IdleTime = 0.f; }

public: // Interfaces

	//~ Begin UObject Interface
	void PostLoad() override;
	//~ End UObject Interface

	//~ Begin UActorComponent Interface
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent Interface

	//~ Begin UMovementComponent Interface
	void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	void HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) override;
	//~ End UMovementComponent Interface

public: // Properties

	/** Should this be from physical material? */
	UPROPERTY(EditAnywhere)
	float Restitution;

	UPROPERTY(EditAnywhere)
	float FixedTimestep;

	UPROPERTY(Transient)
	FHitResult FloorHit;

	UPROPERTY()
	int32 NumSimulatedFrames;

	UPROPERTY()
	int32 SimulatingMoveId;

private: // Functions

	FVector ComputeBounceVelocity(const FHitResult& InHit);
	void AccumulateRoll(const FVector& InOldLocation, const FVector& InNewLocation, const FVector& InRollUp);
	void UpdateFloor();

private: // Properties

	UPROPERTY(Transient, DuplicateTransient)
	class AJolfPawn* JolfOwner;

	UPROPERTY(Transient, DuplicateTransient)
	class USphereComponent* SphereUpdatedComponent;

	UPROPERTY()
	float Accumulator;

	UPROPERTY()
	FVector PrevLocation;

	UPROPERTY()
	FQuat PrevQuat;
	
	UPROPERTY()
	FQuat TargetQuat;

	/** Accumulated roll. */
	UPROPERTY()
	FQuat RollQuat;

	UPROPERTY()
	FVector RollAxis;

	UPROPERTY()
	bool bResimulating;

	/** @see: GetIdleTime */
	UPROPERTY(Transient)
	float IdleTime;
};