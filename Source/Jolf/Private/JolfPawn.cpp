// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPawn.h"
#include "Components/InputComponent.h"
#include "Engine/CollisionProfile.h"
#include "Components/StaticMeshComponent.h"
#include "JolfMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "JolfPlayerState.h"
#include "JolfProjectile.h"
#include "NiagaraComponent.h"
#include "JolfPhysicalMaterial.h"
#include "JolfHole.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/Canvas.h"
#include "DisplayDebugHelpers.h"
#include "JolfPowerArrow.h"
#include "JolfPlayerCameraManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "JolfGameState.h"
#include "Engine/World.h"
#include "JolfPlayerController.h"
#include "JolfPerPlayerSettings.h"
#include "JolfSceneProbeSubsystem.h"

DECLARE_LOG_CATEGORY_CLASS(LogJolfPrediction, Log, All)

static bool bDrawPrediction = false;
static FAutoConsoleVariableRef CVarDrawPrediction(TEXT("j.DrawPrediction"), bDrawPrediction, TEXT(""));

#if ENABLE_DRAW_DEBUG
static bool bDrawControlForward = false;
static FAutoConsoleVariableRef CVarDrawControlForward(TEXT("j.DrawControlForward"), bDrawControlForward, TEXT(""));
static bool bDrawControlForward2D = false;
static FAutoConsoleVariableRef CVarDrawControlForward2D(TEXT("j.DrawControlForward2D"), bDrawControlForward2D, TEXT(""));
static bool bDrawControlRight = false;
static FAutoConsoleVariableRef CVarDrawControlRight(TEXT("j.DrawControlRight"), bDrawControlRight, TEXT(""));
static bool bDrawControlRight2D = false;
static FAutoConsoleVariableRef CVarDrawControlRight2D(TEXT("j.DrawControlRight2D"), bDrawControlRight2D, TEXT(""));
static bool bDrawControlDirection = false;
static FAutoConsoleVariableRef CVarDrawControlDirection(TEXT("j.DrawControlDirection"), bDrawControlDirection, TEXT(""));
static bool bDrawMisprediction = false;
static FAutoConsoleVariableRef CVarDrawMisprediction(TEXT("j.DrawMisprediction"), bDrawMisprediction, TEXT(""));
static bool bDrawResimulationDelta = false;
static FAutoConsoleVariableRef CVarDrawResimulationDelta(TEXT("j.DrawResimulationDelta"), bDrawResimulationDelta, TEXT(""));
static bool bDrawClientSimulationHistory = false;
static FAutoConsoleVariableRef CVarDrawClientSimulationHistory(TEXT("j.DrawClientSimulationHistory"), bDrawClientSimulationHistory, TEXT(""));
static bool bDrawServerSimulationFuture = false;
static FAutoConsoleVariableRef CVarDrawServerSimulationFuture(TEXT("j.DrawServerSimulationFuture"), bDrawServerSimulationFuture, TEXT(""));
static float DrawFlybySpeedThreshold = -1.0f;
static FAutoConsoleVariableRef CVarDrawFlybySpeedThreshold(TEXT("j.DrawFlybySpeedThreshold"), DrawFlybySpeedThreshold, TEXT(""));
#endif // ENABLE_DRAW_DEBUG

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
/** Useful for testing client re-simulation. */
static float ServerMoveDelay = -1.f;
static FAutoConsoleVariableRef CVarServerMoveDelay(TEXT("j.ServerMoveDelay"), ServerMoveDelay, TEXT(""));

/** Always re-simulate every client update. */
static bool bForceResimulation = false;
static FAutoConsoleVariableRef CVarForceResimulation(TEXT("j.ForceResimulation"), bForceResimulation, TEXT(""));
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

AJolfPawn::AJolfPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false); // Jolf has custom movement replication.

	BaseEyeHeight = 0.0f;
	bCollideWhenPlacing = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(23.5f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetShouldUpdatePhysicsVolume(true);
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = true;
	CollisionComponent->bReturnMaterialOnMove = true; // Necessary for impact effects.
	CollisionComponent->SetIsReplicated(false); // Jolf has custom movement replication.
	CollisionComponent->bReplicatePhysicsToAutonomousProxy = false;
	RootComponent = CollisionComponent;

	MovementComponent = CreateDefaultSubobject<UJolfMovementComponent>(TEXT("Movement"));
	MovementComponent->UpdatedComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->AlwaysLoadOnClient = true;
	MeshComponent->AlwaysLoadOnServer = true;
	MeshComponent->bCastDynamicShadow = true;
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SpeedNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Speed"));
	SpeedNiagaraComponent->SetupAttachment(RootComponent);

	FlybyAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FlybyAudio"));
	FlybyAudioComponent->bAutoManageAttachment = true;
	FlybyAudioComponent->AutoAttachParent = MeshComponent;
	FlybyAudioComponent->bAutoActivate = false;
	FlybyAudioComponent->PitchModulationMin = 0.95f;
	FlybyAudioComponent->PitchModulationMax = 1.05f;
	FlybyAudioComponent->VolumeModulationMin = 0.95f;
	FlybyAudioComponent->VolumeModulationMax = 1.05f;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(MeshComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	SceneProbeBrushComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SceneProbeBrush"));
	SceneProbeBrushComponent->SetupAttachment(MeshComponent);
	SceneProbeBrushComponent->SetAbsolute(false, true, false);
	SceneProbeBrushComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	NormalizedZoom = 0.5f;
	SpringArmZoomLength = FFloatInterval(150.f, 600.f);
	SpringArmZoomOffset = FFloatInterval(30.f, 220.f);

	ServerSimulationFutureResult.SimulationIndex = INDEX_NONE;
	PawnCollisionResponse = 3; // Invalid initial value to force replication.

	SquashAndStretchSpring.SetDefaultSpringConstants(10.f, 0.5f);
}

// Public Functions:

void AJolfPawn::OnMovementImpact(const FHitResult& InHitResult)
{
	if (IsNetMode(ENetMode::NM_DedicatedServer))
		return;

	const float VelocityAlongNormal = FVector::DotProduct(MovementComponent->Velocity, InHitResult.Normal);
	if (VelocityAlongNormal > -100.f)
		return;

	const float SpeedAlongNormal = FMath::Abs(VelocityAlongNormal);

	const UJolfPhysicalMaterial* PhysMat = Cast<UJolfPhysicalMaterial>(InHitResult.PhysMaterial.Get());
	if (PhysMat)
	{
		if (PhysMat->ImpactSound)
		{
			const float VolumeMultiplier = PhysMat->ImpactSpeedVolume.GetRichCurveConst()->Eval(SpeedAlongNormal);
			const float PitchMultiplier = FMath::RandRange(0.95f, 1.05f);
			UGameplayStatics::PlaySoundAtLocation(this, PhysMat->ImpactSound, InHitResult.ImpactPoint, VolumeMultiplier, PitchMultiplier);
		}

		if (PhysMat->SplashSystem)
		{
			FQuat Quat = FRotationMatrix::MakeFromZ(InHitResult.Normal).ToQuat();
			Quat = FQuat(InHitResult.Normal, FMath::FRand() * PI * 2.f) * Quat; // Random roll around the normal.
			const FRotator Rotation = Quat.Rotator();

			UNiagaraComponent* SplashInstance = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,
				PhysMat->SplashSystem,
				InHitResult.ImpactPoint,
				Rotation,
				FVector::OneVector, 
				/*bAutoDestroy*/ true, 
				/*bAutoActivate*/ true, 
				ENCPoolMethod::AutoRelease);
			if (ensure(SplashInstance))
			{
				SplashInstance->SetVariableFloat(TEXT("User.Speed"), SpeedAlongNormal);

				// Inherit velocity perpendicular to the surface normal.
				// Experimented with setting ComponentVelocity, but it was not picked up by the system.
				SplashInstance->SetVariableVec3(TEXT("User.InheritVelocity"), MovementComponent->Velocity + InHitResult.Normal * -VelocityAlongNormal);
			}
		}
	}

	SquashAndStretchDirection = InHitResult.ImpactNormal;
	float TargetSquashAndStretch = SquashAndStretchImpactSpeedCurve.GetRichCurveConst()->Eval(SpeedAlongNormal);
	SquashAndStretchSpring.SetPosition(TargetSquashAndStretch);
}

void AJolfPawn::OnHoleOverlap(class AJolfHole* InHole)
{
	AJolfPlayerState* JolfPlayerState = GetPlayerState<AJolfPlayerState>();
	JolfPlayerState->SetHoleScore(JolfPlayerState->HoleIndex, JolfPlayerState->StrokeIndex, PlayTime);

	RepScore.WeakOverlappingHole = InHole;
	RepScore.NumStrokes = JolfPlayerState->StrokeIndex;
	RepScore.Time = PlayTime;
	OnRep_Score();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(HoleCompletionTimer, this, &AJolfPawn::OnHoleCompletionTimerFired, 2.0f);
	}
}

void AJolfPawn::RecordClientSimulationResult()
{
	const FVector ClientLocation = RootComponent->GetComponentLocation();
	const FVector& ClientVelocity = MovementComponent->Velocity;

	if (ServerSimulationFutureResult.SimulationIndex != INDEX_NONE && ServerSimulationFutureResult.SimulationIndex == MovementComponent->NumSimulatedFrames)
	{
		const FVector LocationDelta = ClientLocation - ServerSimulationFutureResult.Location;
		const FVector VelocityDelta = ClientVelocity - ServerSimulationFutureResult.Velocity;
		if (LocationDelta.IsNearlyZero() && VelocityDelta.IsNearlyZero())
		{
			UE_LOG(LogJolfPrediction, Verbose, TEXT("Client matched server future (Index: %i MoveId: %i)"), ServerSimulationFutureResult.SimulationIndex, ServerSimulationFutureResult.MoveId);
		}
		else
		{
			UE_LOG(LogJolfPrediction, Warning, TEXT("Client did not match server future LocationDelta: %s VelocityDelta: %s (Index: %i MoveId: %i)"), *LocationDelta.ToCompactString(), *VelocityDelta.ToCompactString(), ServerSimulationFutureResult.SimulationIndex, ServerSimulationFutureResult.MoveId);

#if ENABLE_DRAW_DEBUG
			if (bDrawMisprediction)
			{
				// Red location sphere and velocity arrow for the client result.
				DrawDebugSphere(GetWorld(), ClientLocation, CollisionComponent->GetScaledSphereRadius(), 16, FColor::Red, false, 1.f);
				DrawDebugDirectionalArrow(GetWorld(),
					ClientLocation,
					ClientLocation + ClientVelocity.GetSafeNormal() * 100.f,
					25.f, FColor::Red, /*bPersistentLines*/ false, 1.f);

				// Green location sphere and velocity arrow for the server result.
				DrawDebugSphere(GetWorld(), ServerSimulationFutureResult.Location, CollisionComponent->GetScaledSphereRadius(), 16, FColor::Green, false, 1.f);
				DrawDebugDirectionalArrow(GetWorld(),
					ServerSimulationFutureResult.Location,
					ServerSimulationFutureResult.Location + ServerSimulationFutureResult.Velocity.GetSafeNormal() * 100.f,
					25.f, FColor::Green, /*bPersistentLines*/ false, 1.f);
			}
#endif // ENABLE_DRAW_DEBUG

			RootComponent->SetWorldLocation(ServerSimulationFutureResult.Location);
			MovementComponent->Velocity = ServerSimulationFutureResult.Velocity;

			// This is the most recent result, so resimulation is unnecessary.
		}

		ClientSimulationHistory.Reset();
		ServerSimulationFutureResult.SimulationIndex = INDEX_NONE;
	}
	else
	{
		FJolfPawnHistory& History = ClientSimulationHistory.AddDefaulted_GetRef();
		History.Location = ClientLocation;
		History.Velocity = ClientVelocity;
		History.SimulationIndex = MovementComponent->NumSimulatedFrames;
	}
}

float AJolfPawn::GetIdleTime() const
{
	return MovementComponent->GetIdleTime();
}

void AJolfPawn::OnEnablePawnCollisionChanged(bool bInEnablePawnCollision)
{
	if (bInEnablePawnCollision)
	{
		// Overlap because all pawns spawn at the same start. Response changes to block after overlap count is zero.
		bShouldTickPawnCollisionResponse = true;
		PawnCollisionResponse = static_cast<uint8>(ECollisionResponse::ECR_Overlap);
	}
	else
	{
		// Response will not change to block, so immediately ignore.
		bShouldTickPawnCollisionResponse = false;
		PawnCollisionResponse = static_cast<uint8>(ECollisionResponse::ECR_Ignore);
	}
	OnRep_PawnCollisionResponse();
}

void AJolfPawn::FakeInput(const FRotator& InControlRotation, float InPower)
{
	if (Controller)
	{
		Controller->SetControlRotation(InControlRotation);
	}

	SetIsLaunching(true);
	InputStrength = InPower;
	OnLaunchPressed();
}

// Public Interfaces:

//~ Begin UObject Interface
void AJolfPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AJolfPawn, RepScore);
	DOREPLIFETIME(AJolfPawn, RepMove);
	DOREPLIFETIME(AJolfPawn, ServerSimulationResult);
	DOREPLIFETIME(AJolfPawn, PawnCollisionResponse);
	DOREPLIFETIME(AJolfPawn, bWantsToFinishHole);
	DOREPLIFETIME(AJolfPawn, bCanMove);
	DISABLE_REPLICATED_PROPERTY_FAST(AActor, AttachmentReplication);
	DISABLE_REPLICATED_PROPERTY_FAST(AActor, ReplicatedMovement);
}
//~ End UObject Interface

//~ Begin AActor Interface
FVector AJolfPawn::GetVelocity() const
{
	return MovementComponent->Velocity;
}

void AJolfPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (HasAuthority())	
	{
		// GameState can be null here in blueprint editor.
		if (const AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
		{
			OnEnablePawnCollisionChanged(GameState->bEnablePawnCollision);
		}
	}

	DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(0);

	if (UJolfSceneProbeSubsystem* Subsystem = GetWorld()->GetSubsystem<UJolfSceneProbeSubsystem>())
	{
		Subsystem->AddBrushComponent(SceneProbeBrushComponent);
	}
}

void AJolfPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
#if ENABLE_DRAW_DEBUG
	if (bDrawControlForward)
	{
		const FVector ControlForward = GetControlRotation().RotateVector(FVector::ForwardVector);
		DrawDebugDirectionalArrow(GetWorld(),
			MeshComponent->GetComponentLocation(),
			MeshComponent->GetComponentLocation() + ControlForward * 200.f,
			25.f,
			FColor::Red);
	}
	if (bDrawControlForward2D)
	{
		const FVector ControlForward = GetControlRotation().RotateVector(FVector::ForwardVector);
		const FVector ControlForward2D = ControlForward.GetSafeNormal2D();
		DrawDebugDirectionalArrow(GetWorld(),
			MeshComponent->GetComponentLocation(),
			MeshComponent->GetComponentLocation() + ControlForward2D * 200.f,
			25.f,
			FColor::Red);
	}
	if (bDrawControlRight)
	{
		const FVector ControlRight = GetControlRotation().RotateVector(FVector::RightVector);
		DrawDebugDirectionalArrow(GetWorld(),
			MeshComponent->GetComponentLocation(),
			MeshComponent->GetComponentLocation() + ControlRight * 200.f,
			25.f,
			FColor::Green);
	}
	if (bDrawControlRight2D)
	{
		const FVector ControlRight = GetControlRotation().RotateVector(FVector::RightVector);
		const FVector ControlRight2D = ControlRight.GetSafeNormal2D();
		DrawDebugDirectionalArrow(GetWorld(),
			MeshComponent->GetComponentLocation(),
			MeshComponent->GetComponentLocation() + ControlRight2D * 200.f,
			25.f,
			FColor::Green);
	}
	if (bDrawControlDirection)
	{
		const FVector ControlForward = GetControlDirection(GetControlRotation(), InputPitch);
		DrawDebugDirectionalArrow(GetWorld(),
			MeshComponent->GetComponentLocation(),
			MeshComponent->GetComponentLocation() + ControlForward * 200.f,
			25.f,
			FColor::Red);
	}
	if (bDrawClientSimulationHistory)
	{
		for (int32 Index = 1; Index < ClientSimulationHistory.Num(); ++Index)
		{
			const FJolfPawnHistory& Prev = ClientSimulationHistory[Index - 1];
			const FJolfPawnHistory& Next = ClientSimulationHistory[Index];
			DrawDebugLine(GetWorld(), Prev.Location, Next.Location, FColor::Red, false, -1.f, SDPG_World, 5.f);
		}
	}
	if (bDrawServerSimulationFuture)
	{
		if (ServerSimulationFutureResult.SimulationIndex != INDEX_NONE)
		{
			DrawDebugSphere(GetWorld(), ServerSimulationFutureResult.Location, CollisionComponent->GetScaledSphereRadius(), 16, FColor::Green);
		}
	}
#endif // ENABLE_DRAW_DEBUG

	if (AJolfHole* OverlappingHole = RepScore.WeakOverlappingHole.Get())
	{
		const FVector CurrentLocation = SpringArmComponent->GetComponentLocation();
		const FVector TargetLocation = OverlappingHole->GetActorLocation();
		FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaSeconds, 4.f);
		SpringArmComponent->SetWorldLocation(NewLocation);
	}
	else
	{
		if (const AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
		{
			if (!GameState->bSequentialTurns || GetPlayerState() == GameState->ActivePlayerState)
			{
				PlayTime += DeltaSeconds;
			}
		}
	}

	if (IsLaunching())
	{
		const FVector Direction = GetControlDirection(GetControlRotation(), InputPitch);

		if (ArrowInstance)
		{
			const FVector ArrowLocation = CollisionComponent->GetComponentLocation() - FVector(0.f, 0.f, CollisionComponent->GetScaledSphereRadius());
			const FQuat ArrowQuat = FRotationMatrix::MakeFromX(Direction).ToQuat();
			ArrowInstance->SetPower(InputStrength);
			ArrowInstance->SetActorLocationAndRotation(ArrowLocation, ArrowQuat);
			
			if (ShadowArrowInstance)
			{
				const FVector ShadowDirection = GetControlDirection(GetControlRotation(), 0.f);
				const FQuat ShadowArrowQuat = FRotationMatrix::MakeFromX(ShadowDirection).ToQuat();

				ShadowArrowInstance->SetPower(InputStrength);
				ShadowArrowInstance->SetActorLocationAndRotation(ArrowLocation, ShadowArrowQuat);
			}
		}

		if (APlayerController* PC = GetController<APlayerController>())
		{
			if (AJolfPlayerCameraManager* PCM = Cast<AJolfPlayerCameraManager>(PC->PlayerCameraManager))
			{
				PCM->SetPower(InputStrength);
			}
		}

		if (bDrawPrediction)
		{
			if (Projectile == nullptr)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.bNoFail = true;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Instigator = this;
				SpawnParams.Owner = this;
				SpawnParams.ObjectFlags |= RF_Transient;
				Projectile = GetWorld()->SpawnActor<AJolfProjectile>(ProjectileClass, SpawnParams);
				Projectile->GetCollisionComponent()->SetSphereRadius(CollisionComponent->GetUnscaledSphereRadius());
			}

			Projectile->GetCollisionComponent()->SetWorldLocation(CollisionComponent->GetComponentLocation(), false, nullptr, ETeleportType::ResetPhysics);
			Projectile->GetGolfMovementComponent()->Velocity = MovementComponent->Velocity + Direction * (InputStrength * 2500.f);
			for (int32 Step = 0; Step < 250; ++Step)
			{
				const FVector LineStart = Projectile->GetCollisionComponent()->GetComponentLocation();
				Projectile->GetGolfMovementComponent()->SimulateProjectile(MovementComponent->FixedTimestep);
				const FVector LineEnd = Projectile->GetCollisionComponent()->GetComponentLocation();
				DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::White);
				DrawDebugPoint(GetWorld(), LineEnd, 10.f, FColor::White);

				if (Projectile->GetGolfMovementComponent()->Velocity.IsNearlyZero())
					break;
			}
		}
		else
		{
			if (Projectile != nullptr)
			{
				Projectile->Destroy();
				Projectile = nullptr;
			}
		}
	}

	if (!IsNetMode(ENetMode::NM_DedicatedServer))
	{
		FVector VelDir;
		float VelSize;
		MovementComponent->Velocity.ToDirectionAndLength(VelDir, VelSize);

		for (const TTuple<UJolfPhysicalMaterial*, UNiagaraComponent*>& Comp : GroundTrailNiagaraComponents)
		{
			Comp.Value->SetVariableFloat(TEXT("User.Speed"), 0.0f);
		}

		if (MovementComponent->FloorHit.IsValidBlockingHit())
		{
			UJolfPhysicalMaterial* PhysMat = Cast<UJolfPhysicalMaterial>(MovementComponent->FloorHit.PhysMaterial.Get());
			if (PhysMat)
			{
				UNiagaraComponent*& Comp = GroundTrailNiagaraComponents.FindOrAdd(PhysMat);
				if (!Comp)
				{ 
					Comp = NewObject<UNiagaraComponent>(this);
					Comp->SetupAttachment(CollisionComponent);
					Comp->SetRelativeLocation(FVector(0.f, 0.f, -CollisionComponent->GetScaledSphereRadius()));
					Comp->SetAsset(PhysMat->TrailSystem);
					Comp->RegisterComponent();
				}

				Comp->SetWorldRotation(FRotationMatrix::MakeFromX(VelDir).ToQuat());
				Comp->SetVariableFloat(TEXT("User.Speed"), VelSize);
			}
		}

		SpeedNiagaraComponent->SetWorldRotation(FRotationMatrix::MakeFromX(VelDir).ToQuat());
		SpeedNiagaraComponent->SetVariableFloat(TEXT("User.Speed"), VelSize);

		float SmearScale = SpeedSmearCurve.GetRichCurveConst()->Eval(VelSize);
		DynamicMaterial->SetVectorParameterValue(TEXT("Smear"), FLinearColor(VelDir.X, VelDir.Y, VelDir.Z, SmearScale));
		float CurrentSquashAndStretch = SquashAndStretchSpring.Update(0.f, DeltaSeconds);
		DynamicMaterial->SetVectorParameterValue(TEXT("SquashAndStretch"), FLinearColor(SquashAndStretchDirection.X, SquashAndStretchDirection.Y, SquashAndStretchDirection.Z, CurrentSquashAndStretch));
	
		SceneProbeBrushComponent->SetVariableBool(TEXT("User.bPlanted"), MovementComponent->FloorHit.IsValidBlockingHit());
	
		TickFlybyAudio(DeltaSeconds);
	}

	if (bShouldTickPawnCollisionResponse)
	{
		bool bOverlappingAnyPawns = false;
		const TArray<FOverlapInfo>& Overlaps = CollisionComponent->GetOverlapInfos();
		for (const FOverlapInfo& Overlap : Overlaps)
		{
			if (APawn* OverlapPawn = Cast<APawn>(Overlap.OverlapInfo.Actor.Get()))
			{
				bOverlappingAnyPawns = true;
				break;
			}
		}
			
		if (!bOverlappingAnyPawns)
		{
			bShouldTickPawnCollisionResponse = false;
			PawnCollisionResponse = static_cast<uint8>(ECollisionResponse::ECR_Block);
			OnRep_PawnCollisionResponse();
		}
	}

	if (HasAuthority())
	{
		bCanMove = CanLaunch();
	}
}

void AJolfPawn::BecomeViewTarget(class APlayerController* PC)
{
	Super::BecomeViewTarget(PC);
	
	if (PC->IsLocalPlayerController())
	{
		if (PC->GetPawn() == this)
		{
			UUserWidget* PawnWidgetInstance = CreateWidget<UUserWidget>(PC, PawnWidgetClass);
			if (ensure(PawnWidgetInstance))
			{
				PawnWidgetInstanceMap.Add(PC, PawnWidgetInstance);
				PawnWidgetInstance->AddToPlayerScreen(10);
			}

			SynchronizeSpringArmZoom();

			FActorSpawnParameters SpawnParams;
			SpawnParams.bNoFail = true;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Instigator = this;
			SpawnParams.Owner = this;
			SpawnParams.ObjectFlags |= RF_Transient;
			ArrowInstance = GetWorld()->SpawnActor<AJolfPowerArrow>(ArrowClass, SpawnParams);
			ArrowInstance->SetActorHiddenInGame(true);
			ArrowInstance->GetRootComponent()->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ArrowInstance->SetColorArrowMask();
			ShadowArrowInstance = GetWorld()->SpawnActor<AJolfPowerArrow>(ArrowClass, SpawnParams);
			ShadowArrowInstance->SetActorHiddenInGame(true);
			ShadowArrowInstance->GetRootComponent()->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ShadowArrowInstance->SetShadowArrowMask();
		}
		else
		{
			UUserWidget* SpectateWidgetInstance = CreateWidget<UUserWidget>(PC, SpectateWidgetWidgetClass);
			if (ensure(SpectateWidgetInstance))
			{
				SpectateWidgetInstanceMap.Add(PC, SpectateWidgetInstance);
				SpectateWidgetInstance->AddToPlayerScreen(10);
			}
		}
	}
}

void AJolfPawn::EndViewTarget(class APlayerController* PC)
{
	Super::EndViewTarget(PC);
	
	if (PC->IsLocalPlayerController())
	{
		UUserWidget* PawnWidgetInstance;
		if (PawnWidgetInstanceMap.RemoveAndCopyValue(PC, PawnWidgetInstance))
		{
			PawnWidgetInstance->RemoveFromViewport();
			PawnWidgetInstance = nullptr;
		}

		UUserWidget* HoleScoreInstance;
		if (HoleScoreInstanceMap.RemoveAndCopyValue(PC, HoleScoreInstance))
		{
			HoleScoreInstance->RemoveFromViewport();
			HoleScoreInstance = nullptr;
		}

		UUserWidget* SpectateWidgetInstance;
		if (SpectateWidgetInstanceMap.RemoveAndCopyValue(PC, SpectateWidgetInstance))
		{
			SpectateWidgetInstance->RemoveFromViewport();
			SpectateWidgetInstance = nullptr;
		}
	}
}

void AJolfPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	CameraComponent->GetCameraView(DeltaTime, OutResult);
}

void AJolfPawn::FellOutOfWorld(const class UDamageType& dmgType)
{
	// Do not call super yet because it destroys the actor.
	if (HasAuthority())
	{
		if (AController* PlayerController = GetController<APlayerController>())
		{
			if (MulliganStack.Num() > 0)
			{
				ApplyMulligan();
			}
			else
			{
				PlayerController->StartSpot.Reset();
				Destroy();
				// Restart AFTER destroy otherwise existing pawn is reused.
				GetWorld()->GetAuthGameMode()->RestartPlayer(PlayerController);
			}
		}
		else
		{
			// Not controlled? Fallback to destroying.
			Super::FellOutOfWorld(dmgType);
		}
	}
}

void AJolfPawn::DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
	
	float Indent = 0.0f;

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;
	DisplayDebugManager.SetDrawColor(FColor::White);

	DisplayDebugManager.DrawString(FString::Printf(TEXT("Velocity: %s Speed: %f"), *MovementComponent->Velocity.ToCompactString(), MovementComponent->Velocity.Size()));
	
	if (MovementComponent->FloorHit.IsValidBlockingHit())
	{
		DisplayDebugManager.DrawString(TEXT("Floor"));
		FIndenter Indenter(Indent);
		DisplayDebugManager.DrawString(FString::Printf(TEXT("PhysMat: %s"), *GetNameSafe(MovementComponent->FloorHit.PhysMaterial.Get())), Indent);
		DisplayDebugManager.DrawString(FString::Printf(TEXT("Actor: %s"), *GetNameSafe(MovementComponent->FloorHit.GetActor())), Indent);
		DisplayDebugManager.DrawString(FString::Printf(TEXT("Component: %s"), *GetNameSafe(MovementComponent->FloorHit.GetComponent())), Indent);
		DisplayDebugManager.DrawString(FString::Printf(TEXT("Bone: %s"), *MovementComponent->FloorHit.BoneName.ToString()), Indent);
	}
}

void AJolfPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UJolfSceneProbeSubsystem* Subsystem = GetWorld()->GetSubsystem<UJolfSceneProbeSubsystem>())
	{
		Subsystem->RemoveBrushComponent(SceneProbeBrushComponent);
	}

	Super::EndPlay(EndPlayReason);
}
//~ End AActor Interface

// Protected Interfaces:

//~ Begin APawn Interface
void AJolfPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Launch"), EInputEvent::IE_Pressed, this, &AJolfPawn::OnLaunchPressed);
	PlayerInputComponent->BindAction(TEXT("Cancel"), EInputEvent::IE_Pressed, this, &AJolfPawn::OnCancelPressed);
	PlayerInputComponent->BindAction(TEXT("Mulligan"), EInputEvent::IE_Pressed, this, &AJolfPawn::OnMulliganPressed);
	PlayerInputComponent->BindAction(TEXT("StrokePitch"), EInputEvent::IE_Pressed, this, &AJolfPawn::OnStrokePitchPressed);
	PlayerInputComponent->BindAction(TEXT("StrokePitch"), EInputEvent::IE_Released, this, &AJolfPawn::OnStrokePitchReleased);
	PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &AJolfPawn::OnPitchInput);
	PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &AJolfPawn::OnYawInput);
	PlayerInputComponent->BindAxis(TEXT("MousePower"), this, &AJolfPawn::OnMouseStrengthInput);
	PlayerInputComponent->BindAxis(TEXT("TurnPitchAtRate"), this, &AJolfPawn::OnTurnPitchAtRateInput);
	PlayerInputComponent->BindAxis(TEXT("TurnYawAtRate"), this, &AJolfPawn::OnTurnYawAtRateInput);
	PlayerInputComponent->BindAxis(TEXT("GamepadPower"), this, &AJolfPawn::OnGamepadStrengthInput);
	PlayerInputComponent->BindAxis(TEXT("Zoom"), this, &AJolfPawn::OnZoomAxisInput);
}
//~ End APawn Interface

// Private Functions:

void AJolfPawn::ServerMove_Implementation(const FRotator& InControlRotation, float InPower, float InPitch)
{
	if (!CanLaunch())
		return;

	if (AJolfPlayerState* State = GetPlayerState<AJolfPlayerState>())
	{
		++State->StrokeIndex;
	}

	bUsedSequentialTurnMove = true;

	FJolfPawnMulliganState MulliganState;
	MulliganState.Location = GetActorLocation();
	MulliganState.Velocity = MovementComponent->Velocity;
	MulliganState.ControlRotation = InControlRotation;
	MulliganStack.Push(MulliganState);

	const FVector Direction = GetControlDirection(InControlRotation, InPitch);

	RepMove.Location = RootComponent->GetComponentLocation();
	RepMove.Velocity = MovementComponent->Velocity + Direction * (InPower * 2500.f);
	++RepMove.MoveId;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (ServerMoveDelay > 0.f)
	{
		GetWorldTimerManager().SetTimer(DebugServerMoveTimer, this, &AJolfPawn::OnRep_Move, ServerMoveDelay);
		return;
	}
#endif !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	OnRep_Move();
}

void AJolfPawn::ApplyMulligan()
{
	check(MulliganStack.Num() > 0);

	FJolfPawnMulliganState MulliganState = MulliganStack.Pop(/*bAllowShrinking*/ false);
	SetActorLocation(MulliganState.Location);
	MovementComponent->Velocity = MulliganState.Velocity;
	MovementComponent->ResetIdleTime();
	GetController()->SetControlRotation(MulliganState.ControlRotation);
}

void AJolfPawn::ServerMulligan_Implementation()
{
	if (MulliganStack.Num() < 1)
		return;

	AJolfPlayerState* JolfPlayerState = GetPlayerState<AJolfPlayerState>();
	AJolfGameState* JolfGameState = GetWorld()->GetGameState<AJolfGameState>();
	if (JolfPlayerState == nullptr || JolfGameState == nullptr)
		return;

	if (JolfPlayerState->NumMulligansUsed < JolfGameState->NumFreeMulligans)
	{
		++JolfPlayerState->NumMulligansUsed;
		--JolfPlayerState->StrokeIndex;
		ApplyMulligan();
	}
}

void AJolfPawn::OnRep_Score()
{
	if (APlayerController* PlayerController = GetController<APlayerController>())
	{
		if (PlayerController->IsLocalPlayerController())
		{
			UUserWidget* HoleScoreInstance = CreateWidget<UUserWidget>(PlayerController, HoleScoreClass);
			if (ensure(HoleScoreInstance))
			{
				HoleScoreInstanceMap.Add(PlayerController, HoleScoreInstance);
				HoleScoreInstance->AddToPlayerScreen(100);
			}
		}
	}

	if (GoalSound)
	{
		const float VolumeMultiplier = FMath::RandRange(0.95f, 1.05f);
		const float PitchMultiplier = FMath::RandRange(0.95f, 1.05f);
		UGameplayStatics::PlaySoundAtLocation(this, GoalSound, GetActorLocation(), VolumeMultiplier, PitchMultiplier);
	}

	SpringArmComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AJolfPawn::OnRep_Move()
{
	if (RepMove.MoveId < 1)
	{
		// Ignore initial replication of empty move, otherwise client mispredicts immediately.
		return;
	}

	// If client locally predicted the move we resimulate in case the server values were different.
	const int32 NumFramesToResimulate = MovementComponent->SimulatingMoveId == RepMove.MoveId ? MovementComponent->NumSimulatedFrames : 0;

	SetActorLocation(RepMove.Location);
	MovementComponent->Velocity = RepMove.Velocity;
	MovementComponent->ResetIdleTime();
	MovementComponent->NumSimulatedFrames = 0;
	MovementComponent->SimulatingMoveId = RepMove.MoveId;
	ClientSimulationHistory.Reset();
	ServerSimulationFutureResult.SimulationIndex = INDEX_NONE;

	if (NumFramesToResimulate > 0)
	{
		MovementComponent->Resimulate(NumFramesToResimulate);
	}
}

void AJolfPawn::OnRep_ServerSimulationResult()
{
	if (ServerSimulationResult.MoveId < MovementComponent->SimulatingMoveId)
	{
		UE_LOG(LogJolfPrediction, Verbose, TEXT("Client ignoring server result (%i) for past MoveId: %i"), ServerSimulationResult.SimulationIndex, ServerSimulationResult.MoveId);
		return;
	}
	else if (ServerSimulationResult.MoveId > MovementComponent->SimulatingMoveId)
	{
		// Considered assigning future result here, but when OnRep_Move is called the client will be perfectly synced anyway.
		UE_LOG(LogJolfPrediction, Verbose, TEXT("Client ignoring server result (%i) for future MoveId: %i"), ServerSimulationResult.SimulationIndex, ServerSimulationResult.MoveId);
		return;
	}

	if (!bHasInitialServerState)
	{
		// Snap immediately to initial server values.
		
		SetActorLocation(ServerSimulationResult.Location);
		MovementComponent->Velocity = ServerSimulationResult.Velocity;
		MovementComponent->ResetIdleTime();
		MovementComponent->NumSimulatedFrames = ServerSimulationResult.SimulationIndex + 1;
		MovementComponent->SimulatingMoveId = RepMove.MoveId;
		ClientSimulationHistory.Reset();
		ServerSimulationFutureResult.SimulationIndex = INDEX_NONE;

		bHasInitialServerState = true;
		return;
	}

	if (ClientSimulationHistory.Num() > 0 && ServerSimulationResult.SimulationIndex <= ClientSimulationHistory.Last().SimulationIndex)
	{
		const int32 HistoryIndex = ClientSimulationHistory.IndexOfByPredicate([this](const FJolfPawnHistory& Entry)
		{
			return Entry.SimulationIndex == ServerSimulationResult.SimulationIndex;
		});

		if (HistoryIndex != INDEX_NONE)
		{
			FJolfPawnHistory HistoryEntry = ClientSimulationHistory[HistoryIndex];

			const int32 ExpectedHistoryLength = ClientSimulationHistory.Num() - HistoryIndex - 1;

			const FVector LocationDelta = HistoryEntry.Location - ServerSimulationResult.Location;
			const FVector VelocityDelta = HistoryEntry.Velocity - ServerSimulationResult.Velocity;
			if (LocationDelta.IsNearlyZero() 
				&& VelocityDelta.IsNearlyZero()
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				&& !bForceResimulation
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
				)
			{
				UE_LOG(LogJolfPrediction, Verbose, TEXT("Client matched server (Index: %i MoveId: %i)"), ServerSimulationResult.SimulationIndex, ServerSimulationResult.MoveId);

				// Discard validated segment of history.
				ClientSimulationHistory.RemoveAt(0, HistoryIndex + 1, /*bAllowShrinking*/ false);
			}
			else
			{
				UE_LOG(LogJolfPrediction, Warning, TEXT("Client did not match server LocationDelta: %s VelocityDelta: %s (Index: %i MoveId: %i)"), *LocationDelta.ToCompactString(), *VelocityDelta.ToCompactString(), ServerSimulationResult.SimulationIndex, ServerSimulationResult.MoveId);

#if ENABLE_DRAW_DEBUG
				if (bDrawMisprediction)
				{
					// Red location sphere and velocity arrow for the client result.
					DrawDebugSphere(GetWorld(), HistoryEntry.Location, CollisionComponent->GetScaledSphereRadius(), 16, FColor::Red, false, 1.f);
					DrawDebugDirectionalArrow(GetWorld(),
						HistoryEntry.Location,
						HistoryEntry.Location + HistoryEntry.Velocity.GetSafeNormal() * 100.f,
						25.f, FColor::Red, /*bPersistentLines*/ false, 1.f);

					// Green location sphere and velocity arrow for the server result.
					DrawDebugSphere(GetWorld(), ServerSimulationResult.Location, CollisionComponent->GetScaledSphereRadius(), 16, FColor::Green, false, 1.f);
					DrawDebugDirectionalArrow(GetWorld(),
						ServerSimulationResult.Location,
						ServerSimulationResult.Location + ServerSimulationResult.Velocity.GetSafeNormal() * 100.f,
						25.f, FColor::Green, /*bPersistentLines*/ false, 1.f);
				}
				if (bDrawResimulationDelta && ExpectedHistoryLength > 0)
				{
					// Red location sphere and velocity arrow for the old state.
					DrawDebugSphere(GetWorld(), CollisionComponent->GetComponentLocation(), CollisionComponent->GetScaledSphereRadius(), 16, FColor::Red, false, 1.f);
					DrawDebugDirectionalArrow(GetWorld(),
						CollisionComponent->GetComponentLocation(),
						CollisionComponent->GetComponentLocation() + HistoryEntry.Velocity.GetSafeNormal() * 100.f,
						25.f, FColor::Red, /*bPersistentLines*/ false, 1.f);
				}
#endif // ENABLE_DRAW_DEBUG

				RootComponent->SetWorldLocation(ServerSimulationResult.Location);
				MovementComponent->Velocity = ServerSimulationResult.Velocity;

				const int32 HighestSimulationIndex = ClientSimulationHistory.Last().SimulationIndex;

				// Discard validated segment of history, and refill future history.
				ClientSimulationHistory.Reset();

				if (ExpectedHistoryLength > 0)
				{
					UE_LOG(LogJolfPrediction, Verbose, TEXT("Resimulating %i frames"), ExpectedHistoryLength);

					const int32 ExpectedNumSimulatedFrames = MovementComponent->NumSimulatedFrames;

					MovementComponent->NumSimulatedFrames = ServerSimulationResult.SimulationIndex + 1;
					// Catch up to client time.
					MovementComponent->Resimulate(ExpectedHistoryLength);

					check(ClientSimulationHistory.Last().SimulationIndex == HighestSimulationIndex);
					check(MovementComponent->NumSimulatedFrames == ExpectedNumSimulatedFrames);

#if ENABLE_DRAW_DEBUG
					if (bDrawResimulationDelta)
					{
						// Green location sphere and velocity arrow for the new state.
						DrawDebugSphere(GetWorld(), CollisionComponent->GetComponentLocation(), CollisionComponent->GetScaledSphereRadius(), 16, FColor::Green, false, 1.f);
						DrawDebugDirectionalArrow(GetWorld(),
							CollisionComponent->GetComponentLocation(),
							CollisionComponent->GetComponentLocation() + MovementComponent->Velocity.GetSafeNormal() * 100.f,
							25.f, FColor::Green, /*bPersistentLines*/ false, 1.f);
					}
#endif // ENABLE_DRAW_DEBUG
				}
			}

			// Final num should be the same regardless.
			check(ClientSimulationHistory.Num() == ExpectedHistoryLength);
		}
		else
		{
			UE_LOG(LogJolfPrediction, Error, TEXT("Client missing history entry for Index: %i MoveId: %i"), ServerSimulationResult.SimulationIndex, ServerSimulationResult.MoveId);
		}
	}
	else
	{
		if (ServerSimulationFutureResult.SimulationIndex == INDEX_NONE)
		{
			UE_LOG(LogJolfPrediction, Verbose, TEXT("Client saving server result for future comparison (Index: %i MoveId: %i)"), ServerSimulationResult.SimulationIndex, ServerSimulationResult.MoveId);
			ServerSimulationFutureResult = ServerSimulationResult;
		}
	}
}

void AJolfPawn::OnRep_PawnCollisionResponse()
{
	// UE_LOG(LogTemp, Display, TEXT("%i OnRep_PawnCollisionResponse %i"), (int32) GetNetMode(), PawnCollisionResponse);
	if (ensure(PawnCollisionResponse < 3))
	{
		CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, static_cast<ECollisionResponse>(PawnCollisionResponse));
	}
}

void AJolfPawn::OnLaunchPressed()
{
	if (!bCanMove)
	{
		SetIsLaunching(false);
		return;
	}

	if (IsLaunching())
	{
		SetIsLaunching(false);

		if (InputStrength < 0.01f)
		{
			// Cancel probably accidental input.
			return;
		}

		if (StrokeSounds.Num() > 0)
		{
			int32 StrokeSoundIndex = FMath::FloorToInt(InputStrength * StrokeSounds.Num());
			// Clamp otherwise 1.00 is out of bounds.
			StrokeSoundIndex = FMath::Min(StrokeSoundIndex, StrokeSounds.Num() - 1);
			if (class USoundBase* StrokeSound = StrokeSounds[StrokeSoundIndex])
			{
				UGameplayStatics::PlaySoundAtLocation(this, StrokeSound, GetActorLocation(), FMath::RandRange(0.95f, 1.05f), FMath::RandRange(0.95f, 1.05f));
			}
		}

		ServerMove(GetControlRotation(), InputStrength, InputPitch);
	}
	else
	{
		SetIsLaunching(true);
	}
}

void AJolfPawn::OnCancelPressed()
{
	SetIsLaunching(false);
}

void AJolfPawn::OnMulliganPressed()
{
	ServerMulligan();
}

void AJolfPawn::OnStrokePitchPressed()
{
	bStrokePitchHeld = true;
	SynchronizeSpringArmZoom();
}

void AJolfPawn::OnStrokePitchReleased()
{
	bStrokePitchHeld = false;
	SynchronizeSpringArmZoom();
}

void AJolfPawn::OnPitchInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);

		if (!IsLaunching())
		{
			AddControllerPitchInput(InAxisValue * Settings->MousePitchSensitivity);
		}
		else if(bStrokePitchHeld)
		{
			InputPitch += InAxisValue * Settings->MousePitchSensitivity;
			InputPitch = FMath::Clamp(InputPitch, -89.f, +89.f);
			
			if (ShadowArrowInstance)
			{
				ShadowArrowInstance->SetActorHiddenInGame(InputPitch == 0.f);
			}
		}
	}
}

void AJolfPawn::OnYawInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);

		AddControllerYawInput(InAxisValue * Settings->MouseYawSensitivity);
	}
}

void AJolfPawn::OnMouseStrengthInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);

		if (IsLaunching() && !bStrokePitchHeld)
		{
			InputStrength += InAxisValue * Settings->MousePowerSensitivity;
			InputStrength = FMath::Clamp(InputStrength, 0.f, 1.f);
		}
	}
}

void AJolfPawn::OnTurnPitchAtRateInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);

		if (!IsLaunching())
		{
			AddControllerPitchInput(InAxisValue * GetWorld()->GetDeltaSeconds() * CustomTimeDilation * Settings->GamepadPitchDegreesPerSecond);
		}
	}
}

void AJolfPawn::OnTurnYawAtRateInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);

		AddControllerYawInput(InAxisValue * GetWorld()->GetDeltaSeconds() * CustomTimeDilation * Settings->GamepadYawDegreesPerSecond);
	}
}

void AJolfPawn::OnGamepadStrengthInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);

		if (IsLaunching())
		{
			InputStrength -= InAxisValue * Settings->GamepadPowerSensitivity * GetWorld()->GetDeltaSeconds() * CustomTimeDilation;
			InputStrength = FMath::Clamp(InputStrength, 0.f, 1.f);
		}
	}
}

void AJolfPawn::OnZoomAxisInput(float InAxisValue)
{
	NormalizedZoom = FMath::Clamp(NormalizedZoom - InAxisValue, 0.f, 1.f);
	SynchronizeSpringArmZoom();
}

void AJolfPawn::OnHoleCompletionTimerFired()
{
	check(HasAuthority());

	AJolfGameState* JolfGameState = GetWorld()->GetGameState<AJolfGameState>();
	AJolfPlayerState* JolfPlayerState = GetPlayerState<AJolfPlayerState>();

	if (JolfGameState->bShouldWaitForAllPlayersToFinishHole)
	{
		bWantsToFinishHole = true;

		// Automatically change view target.
		AJolfPlayerController* PlayerController = GetController<AJolfPlayerController>();
		PlayerController->ViewAPlayer(+1);
	}
	else
	{
		++JolfPlayerState->HoleIndex;
		JolfPlayerState->StrokeIndex = 0;
		JolfPlayerState->NumMulligansUsed = 0;

		AController* PlayerController = GetController<APlayerController>();
		PlayerController->StartSpot.Reset();
		Destroy();
		GetWorld()->GetAuthGameMode()->RestartPlayer(PlayerController);
	}
}

void AJolfPawn::SynchronizeSpringArmZoom()
{
	SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmZoomLength.Min, SpringArmZoomLength.Max, NormalizedZoom);
	float SocketOffset = FMath::Lerp(SpringArmZoomOffset.Min, SpringArmZoomOffset.Max, NormalizedZoom);
	SpringArmComponent->SocketOffset = FVector(0.f, SocketOffset * bStrokePitchHeld, SocketOffset);
}

FVector AJolfPawn::GetControlDirection(const FRotator& InControlRotation, float InStrokePitch) const
{
	const FVector ControlRight = InControlRotation.RotateVector(FVector::RightVector);
	const FVector FloorNormal = MovementComponent->FloorHit.IsValidBlockingHit() ? MovementComponent->FloorHit.Normal : FVector::UpVector;
	const FVector FloorForward = FVector::CrossProduct(ControlRight, FloorNormal).GetSafeNormal();
	return FloorForward.RotateAngleAxis(-InStrokePitch, ControlRight);
}

bool AJolfPawn::CanLaunch() const
{
	if (RepScore.WeakOverlappingHole.IsValid())
	{
		// Cannot move after landing in hole.
		return false;
	}

	const AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	if (GameState == nullptr)
	{
		// Remaining conditions depend on game state.
		return false;
	}

	if (!GameState->bAllowStrokeWhileMoving && MovementComponent->GetIdleTime() < 0.2f)
	{
		// Cannot move until idle again.
		return false;
	}

	if (GameState->bSequentialTurns)
	{
		if (GameState->ActivePlayerState != GetPlayerState())
		{
			// Not the active player. In earlier version we flagged the active player pawn as allowed to move, but that significantly
			// complicated bookkeeping e.g. if active player respawned during their return.
			return false;
		}

		if (bUsedSequentialTurnMove)
		{
			// Already moved once on this turn. Will be reset once it becomes our turn again.
			return false;
		}
	}

	return true;
}

void AJolfPawn::SetIsLaunching(bool bNewLaunching)
{
	if (bLaunching == bNewLaunching)
		return;

	bLaunching = bNewLaunching;
	if (bLaunching)
	{
		// Canceling launch resets pitch to help with accidental adjustments.
		InputPitch = 0.f;
	}

	if (ArrowInstance)
	{
		ArrowInstance->SetActorHiddenInGame(!IsLaunching());
	}

	if (ShadowArrowInstance)
	{
		ShadowArrowInstance->SetActorHiddenInGame(true);
	}
}

void AJolfPawn::TickFlybyAudio(float InDeltaSeconds)
{
	if (FlybyCooldown > 0.f)
	{
		FlybyCooldown -= InDeltaSeconds;
		return;
	}

	if (FlybySounds.Num() < 1)
	{
		return;
	}

	const FVector Velocity = GetVelocity();
	float SpeedSquared = Velocity.SizeSquared();
	float MinFlybySpeedSquared = FlybySounds[0].MinSpeed * FlybySounds[0].MinSpeed;
	if (SpeedSquared < MinFlybySpeedSquared)
	{
		// Early exit if speed delta cannot possibly exceed minimum.
		return;
	}

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		if ((*Iter)->IsLocalPlayerController() && (*Iter)->GetViewTarget() != this && (*Iter)->PlayerCameraManager)
		{
			const FVector ViewTargetVelocity = (*Iter)->GetViewTarget()->GetVelocity();
			const float SpeedDeltaSquared = (ViewTargetVelocity - Velocity).SizeSquared();
			
#if ENABLE_DRAW_DEBUG
			if (DrawFlybySpeedThreshold > 1.f)
			{
				if (SpeedDeltaSquared > DrawFlybySpeedThreshold * DrawFlybySpeedThreshold)
				{
					DrawDebugSphere(GetWorld(), GetMeshComponent()->GetComponentLocation(), 500.f, 16, FColor::White, false, -1.f, SDPG_World, 2.f);
				}
			}
#endif // ENABLE_DRAW_DEBUG

			if (SpeedDeltaSquared < MinFlybySpeedSquared)
			{
				continue;
			}

			FVector CameraLocation = (*Iter)->PlayerCameraManager->GetCameraLocation();
			if (FVector::DistSquared(CameraLocation, GetMeshComponent()->GetComponentLocation()) < 500.f * 500.f)
			{
				for (int32 SoundIndex = FlybySounds.Num() - 1; SoundIndex >= 0; --SoundIndex)
				{
					const FJolfPawnFlybySound& Item = FlybySounds[SoundIndex];
					if (SpeedDeltaSquared > FMath::Square(Item.MinSpeed))
					{
						FlybyAudioComponent->SetSound(Item.Sound);
						FlybyAudioComponent->Play();
						FlybyCooldown = 2.f;
						return;
					}
				}
			}
		}
	}
}