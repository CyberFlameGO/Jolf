// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "JolfPawn.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if ENABLE_DRAW_DEBUG
static bool bDrawFloorSweep = false;
static FAutoConsoleVariableRef CVarDrawSuspension(TEXT("j.DrawFloorSweep"), bDrawFloorSweep, TEXT(""));

static bool bDrawRollAxis = false;
static FAutoConsoleVariableRef CVarDrawRollAxis(TEXT("j.DrawRollAxis"), bDrawRollAxis, TEXT(""));

static bool bDrawFloorNormal = false;
static FAutoConsoleVariableRef CVarDrawFloorNormal(TEXT("j.DrawFloorNormal"), bDrawFloorNormal, TEXT(""));

static bool bDrawFloorImpactNormal = false;
static FAutoConsoleVariableRef CVarDrawFloorImpactNormal(TEXT("j.DrawFloorImpactNormal"), bDrawFloorImpactNormal, TEXT(""));

static bool bDrawFloorPlane = false;
static FAutoConsoleVariableRef CVarDrawFloorPlane(TEXT("j.DrawFloorPlane"), bDrawFloorPlane, TEXT(""));

static bool bDrawFloorGravity = false;
static FAutoConsoleVariableRef CVarDrawFloorGravity(TEXT("j.DrawFloorGravity"), bDrawFloorGravity, TEXT(""));

static bool bDrawFloorFriction = false;
static FAutoConsoleVariableRef CVarDrawFloorFriction(TEXT("j.DrawFloorFriction"), bDrawFloorFriction, TEXT(""));

static bool bDrawPush = false;
static FAutoConsoleVariableRef CVarDrawPush(TEXT("j.DrawPush"), bDrawPush, TEXT(""));

static float DrawImpactSpeedThreshold = -1.0f;
static FAutoConsoleVariableRef CVarDrawImpactSpeedThreshold(TEXT("j.DrawImpactSpeedThreshold"), DrawImpactSpeedThreshold, TEXT(""));

void DrawDebugSweptSphere(const UWorld* InWorld, FVector const& Start, FVector const& End, float Radius, FColor const& Color)
{
	FVector const TraceVec = End - Start;
	float const Dist = TraceVec.Size();

	FVector const Center = Start + TraceVec * 0.5f;
	float const HalfHeight = (Dist * 0.5f) + Radius;

	FQuat const CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	::DrawDebugCapsule(InWorld, Center, HalfHeight, Radius, CapsuleRot, Color);
}
#endif // ENABLE_DRAW_DEBUG

#if !NO_LOGGING
static bool bLogHandleImpact = false;
static FAutoConsoleVariableRef CVarLogHandleImpact(TEXT("j.LogHandleImpact"), bLogHandleImpact, TEXT(""));
static bool bLogFloor = false;
static FAutoConsoleVariableRef CVarLogFloor(TEXT("j.LogFloor"), bLogFloor, TEXT(""));
static bool bLogFloorChanged = false;
static FAutoConsoleVariableRef CVarLogFloorChanged(TEXT("j.LogFloorChanged"), bLogFloorChanged, TEXT(""));
#endif // !NO_LOGGING

UJolfMovementComponent::UJolfMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Restitution = 0.3f;
	FixedTimestep = 0.1f;

	Accumulator = 0.f;
	TargetQuat = FQuat::Identity;
	RollAxis = FVector::RightVector;
}

// Public Functions:

void UJolfMovementComponent::SimulateProjectile(float InDeltaTime)
{
	if(!GetOwner()->CheckStillInWorld()) // KillZ
		return;

	// In early versions we always applied downward gravity, but it was not ideal for slope friction.
	const float GravityAccel = GetGravityZ() * InDeltaTime;

	UpdateFloor();
	if (FloorHit.IsValidBlockingHit())
	{
		const FVector GravityAccelVector(0.f, 0.f, GravityAccel);
		const float FloorDotGravity = FVector::DotProduct(FloorHit.Normal, GravityAccelVector);

		// Not physically correct, but we do gravity first as an input to friction to ensure ball reaches zero velocity.
		if (!FMath::IsNearlyEqual(FloorDotGravity, GravityAccel))
		{
			const FVector FloorGravity = GravityAccelVector - FloorHit.Normal * FloorDotGravity;
			Velocity += FloorGravity; // Gravity was already multiplied by delta time

#if ENABLE_DRAW_DEBUG
			if (bDrawFloorGravity && !FloorGravity.IsNearlyZero())
			{
				DrawDebugDirectionalArrow(GetWorld(),
					UpdatedComponent->GetComponentLocation(),
					UpdatedComponent->GetComponentLocation() + FloorGravity / InDeltaTime,
					25.f,
					FColor::Blue, false, -1.f, SDPG_World, 2.f);
			}
#endif // ENABLE_DRAW_DEBUG
		}

		if (UPhysicalMaterial* PhysMaterial = FloorHit.PhysMaterial.Get())
		{
			// Force of friction is equal to coefficient of kinetic friction multiplied by normal force. The ball pawn has a mass of one,
			// so acceleration due to friction is equal to coefficient of kinetic friction multiplied by normal acceleration.
			const float FrictionAccel = PhysMaterial->Friction * FloorDotGravity;
			FVector NewVelocity = Velocity.GetClampedToMaxSize(Velocity.Size() + FrictionAccel); // Gravity was already multiplied by delta time

#if ENABLE_DRAW_DEBUG
			if (bDrawFloorFriction)
			{
				FVector VelocityDelta = NewVelocity - Velocity;
				if (!VelocityDelta.IsNearlyZero())
				{
					DrawDebugDirectionalArrow(GetWorld(),
						UpdatedComponent->GetComponentLocation(),
						UpdatedComponent->GetComponentLocation() + VelocityDelta / InDeltaTime,
						25.f,
						FColor::Red, false, -1.f, SDPG_World, 2.f);
				}
			}
#endif // ENABLE_DRAW_DEBUG

			Velocity = NewVelocity;
		}
	}
	else
	{
		Velocity.Z += GravityAccel;
	}
	
	bool bMoved = false;
	FVector Delta = Velocity * InDeltaTime;
	if (!Delta.IsNearlyZero(1e-6f))
	{
		const FVector PreMoveLocation = UpdatedComponent->GetComponentLocation();

		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, FQuat::Identity, true, Hit);
	
		const FVector PostMoveLocation = UpdatedComponent->GetComponentLocation();
		
		const FVector RollUp = FloorHit.IsValidBlockingHit() ? FloorHit.Normal : FVector::UpVector;
		AccumulateRoll(PreMoveLocation, PostMoveLocation, RollUp);
	
		bMoved |= PreMoveLocation != PostMoveLocation;

		if (Hit.IsValidBlockingHit())
		{
			HandleImpact(Hit, InDeltaTime, Delta);

			Delta = Velocity * InDeltaTime;
			// Try to slide the remaining distance along the surface.
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);

			const FVector PostSlideLocation = UpdatedComponent->GetComponentLocation();
			AccumulateRoll(PostMoveLocation, PostSlideLocation, RollUp);
		
			bMoved |= PostMoveLocation != PostSlideLocation;
		}
	}

	IdleTime = (IdleTime + InDeltaTime) * !bMoved;
}

void UJolfMovementComponent::Resimulate(int32 NumFrames)
{
	TGuardValue<bool> ResimulatingGuard(bResimulating, true);

	check(JolfOwner);
	check(!JolfOwner->HasAuthority());

	IdleTime = FMath::Max(0.f, IdleTime - NumFrames * FixedTimestep);
	for (int32 Index = 0; Index < NumFrames; ++Index)
	{
		SimulateProjectile(FixedTimestep);
		JolfOwner->RecordClientSimulationResult();
		++NumSimulatedFrames;
	}
}

// Public Interfaces:

//~ Begin UObject Interface
void UJolfMovementComponent::PostLoad()
{
	Super::PostLoad();
	JolfOwner = Cast<AJolfPawn>(GetOwner());
}
//~ End UObject Interface

//~ Begin UActorComponent Interface
void UJolfMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!UpdatedComponent || !IsValid(JolfOwner))
	{
		return;
	}

	Accumulator += DeltaTime;
	while(Accumulator >= FixedTimestep)
	{
		PrevLocation = UpdatedComponent->GetComponentLocation();
		PrevQuat = TargetQuat;
		RollQuat = FQuat::Identity;
		SimulateProjectile(FixedTimestep);
		TargetQuat = RollQuat * TargetQuat;
		Accumulator -= FixedTimestep;

		if (JolfOwner->HasAuthority())
		{
			JolfOwner->ServerSimulationResult.Location = UpdatedComponent->GetComponentLocation();
			JolfOwner->ServerSimulationResult.Velocity = Velocity;
			JolfOwner->ServerSimulationResult.MoveId = SimulatingMoveId;
			JolfOwner->ServerSimulationResult.SimulationIndex = NumSimulatedFrames;
		}
		else
		{
			JolfOwner->RecordClientSimulationResult();
		}

		++NumSimulatedFrames;
	}
	float InterpAlpha = Accumulator / FixedTimestep;

	// Wait until PrevLocation is initialized before interpolatng.
	if (NumSimulatedFrames > 0)
	{
		const FVector NewLocation = UpdatedComponent->GetComponentLocation();
		FVector TargetLocation = FMath::Lerp(PrevLocation, NewLocation, InterpAlpha);
		FVector RollQuatAxis;
		float RollQuatAngle;
		RollQuat.ToAxisAndAngle(RollQuatAxis, RollQuatAngle);
		FQuat InterpQuat = FQuat(RollQuatAxis, RollQuatAngle * InterpAlpha) * PrevQuat;
		JolfOwner->GetMeshComponent()->SetWorldLocationAndRotation(TargetLocation, InterpQuat);
	}

	// Finalize
	UpdateComponentVelocity();

#if ENABLE_DRAW_DEBUG
	if (bDrawRollAxis)
	{
		::DrawDebugDirectionalArrow(GetWorld(),
			UpdatedComponent->GetComponentLocation(),
			UpdatedComponent->GetComponentLocation() + RollAxis * 100.f,
			25.f,
			FColor::Green);
	}
#endif // ENABLE_DRAW_DEBUG
};
//~ End UActorComponent Interface

//~ Begin UMovementComponent Interface
void UJolfMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);
	JolfOwner = UpdatedComponent ? CastChecked<AJolfPawn>(UpdatedComponent->GetOwner()) : nullptr;
	SphereUpdatedComponent = Cast<USphereComponent>(UpdatedComponent);
}

void UJolfMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
#if !NO_LOGGING
	UE_CLOG(bLogHandleImpact, LogTemp, Display, TEXT("%s HandleImpact %s"), *GetPathName(), *Hit.ToString());
#endif // !NO_LOGGING

#if ENABLE_DRAW_DEBUG
	if (DrawImpactSpeedThreshold > 1.f)
	{
		const float SpeedAlongNormal = FMath::Abs(FVector::DotProduct(Velocity, Hit.Normal));
		if (SpeedAlongNormal > DrawImpactSpeedThreshold)
		{
			DrawDebugDirectionalArrow(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + Hit.Normal * SpeedAlongNormal, DrawImpactSpeedThreshold, FColor::Red, false, 10.f);
			DrawDebugString(GetWorld(), Hit.ImpactPoint, GetNameSafe(Hit.PhysMaterial.Get()), nullptr, FColor::White, 10.f, true);
		}
	}
#endif // ENABLE_DRAW_DEBUG

	if (JolfOwner && !bResimulating)
	{
		JolfOwner->OnMovementImpact(Hit);
	}

	if (!bResimulating && Hit.IsValidBlockingHit())
	{
		if (AJolfPawn* HitPawn = Cast<AJolfPawn>(Hit.GetActor()))
		{
			// Not physically accurate. Should we revise this? Experimented with elastic collisions.
			HitPawn->GetJolfMovementComponent()->Velocity += Hit.Normal * Velocity.Size() * -0.5f;
			
#if ENABLE_DRAW_DEBUG
			if (bDrawPush)
			{
				DrawDebugDirectionalArrow(GetWorld(),
					Hit.ImpactPoint,
					Hit.ImpactPoint + Hit.Normal * -100.f,
					25.f,
					FColor::Red,
					false,
					10.f);
			}
#endif // ENABLE_DRAW_DEBUG
		}
	}

	Velocity = ComputeBounceVelocity(Hit);
}
//~ End UMovementComponent Interface

/** Based on UProjectileMovement::ComputeBounceResult */
FVector UJolfMovementComponent::ComputeBounceVelocity(const FHitResult& InHit)
{
	FVector TempVelocity = Velocity;
	const FVector& Normal = InHit.Normal;
	const float VDotNormal = (TempVelocity | Normal);

	// Only bounce if velocity is opposed by normal.
	// Engine recommends speed threshold of (0.2 * gravity) for stability, but with
	// fixed timestep this was problematic and actually reduced consistency.
	if (VDotNormal < 0.f)
	{
		// Project velocity onto normal in reflected direction.
		const FVector ProjectedNormal = Normal * -VDotNormal;

		// Point velocity in direction parallel to surface
		TempVelocity += ProjectedNormal;

		// Coefficient of restitution only applies perpendicular to impact.
		TempVelocity += ProjectedNormal * Restitution;
	}

	return TempVelocity;
}

void UJolfMovementComponent::AccumulateRoll(const FVector& InOldLocation, const FVector& InNewLocation, const FVector& InRollUp)
{
	if (bResimulating)
		return;

	const FVector LocationDelta = InNewLocation - InOldLocation;
	FVector Direction;
	float ArcLength;
	LocationDelta.ToDirectionAndLength(Direction, ArcLength);
	if(ArcLength < SMALL_NUMBER)
		return;

	// Update RollAxis if RollUp and Direction are not along the same axis (e.g. when falling straight down or 
	// bouncing along normal), otherwise use previous RollAxis value.
	if (FMath::Abs(FVector::DotProduct(InRollUp, Direction)) < (1.f - KINDA_SMALL_NUMBER))
	{
		RollAxis = FVector::CrossProduct(InRollUp, Direction);
		// I always thought cross product of two normal vectors returned a normal vector, but that was incorrect.
		RollAxis = RollAxis.GetSafeNormal();
	}
	
	const float CollisionRadius = SphereUpdatedComponent->GetScaledSphereRadius();
	const float AngleRadians = ArcLength / CollisionRadius;
	const FQuat RotationDelta = FQuat(RollAxis, AngleRadians);
	RollQuat = RotationDelta * RollQuat;
}

void UJolfMovementComponent::UpdateFloor()
{
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	Params.bReturnPhysicalMaterial = true;

	bool bWasFloorValidBlockingHit = FloorHit.IsValidBlockingHit();

	const float CollisionRadius = SphereUpdatedComponent->GetScaledSphereRadius();
	const float SweepRadius = CollisionRadius * 0.95f;
	const float SweepLength = (CollisionRadius - SweepRadius) + 1.f;
	const FVector& SweepStart = UpdatedComponent->GetComponentLocation();
	const FVector SweepEnd = SweepStart + FVector(0.f, 0.f, -SweepLength);
	const bool bFoundFloor = GetWorld()->SweepSingleByChannel(FloorHit,
		SweepStart,
		SweepEnd,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(SweepRadius),
		Params);

#if !NO_LOGGING
	UE_CLOG(bLogFloor, LogTemp, Display, TEXT("%s Floor %s Friction: %f"), *GetPathName(), *FloorHit.ToString(),
		FloorHit.PhysMaterial.IsValid() ? FloorHit.PhysMaterial->Friction : 0.f);
	if (bLogFloorChanged && bWasFloorValidBlockingHit != FloorHit.IsValidBlockingHit())
	{
		UE_LOG(LogTemp, Display, TEXT("%s FloorChanged %s"), *GetPathName(), *FloorHit.ToString());
	}
#endif // !NO_LOGGING

#if ENABLE_DRAW_DEBUG
	if (bDrawFloorSweep)
	{
		::DrawDebugSweptSphere(GetWorld(), SweepStart, SweepEnd, SweepRadius, bFoundFloor ? FColor::Green : FColor::Red);
	}

	if (bDrawFloorNormal && bFoundFloor)
	{
		::DrawDebugDirectionalArrow(GetWorld(), FloorHit.ImpactPoint, FloorHit.ImpactPoint + FloorHit.Normal * 100.f, 25.f, FColor::Blue);
	}
	
	if (bDrawFloorImpactNormal && bFoundFloor)
	{
		::DrawDebugDirectionalArrow(GetWorld(), FloorHit.ImpactPoint, FloorHit.ImpactPoint + FloorHit.ImpactNormal * 100.f, 25.f, FColor::Blue);
	}

	if (bDrawFloorPlane && bFoundFloor)
	{
		::DrawDebugSolidPlane(GetWorld(), FPlane(FloorHit.ImpactPoint, FloorHit.Normal), FloorHit.ImpactPoint, 50.f, FColor::Blue);
	}
#endif // ENABLE_DRAW_DEBUG
}