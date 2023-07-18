// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/Pawn.h"
#include "Engine/SpringInterpolator.h"
#include "JolfPawn.generated.h"

USTRUCT()
struct FJolfPawnMulliganState
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FRotator ControlRotation;
};

USTRUCT()
struct FJolfPawnRepMove
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Velocity;

	/** Used to associate simulation results with a move. */
	UPROPERTY()
	int32 MoveId;
};

USTRUCT()
struct FJolfPawnAuthState
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	int32 SimulationIndex;

	/**
	 * Move this was simulated for.
	 * Allows client to discard result for an old move.
	 */
	UPROPERTY()
	int32 MoveId;
};

USTRUCT()
struct FJolfPawnHistory
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	int32 SimulationIndex;
};

USTRUCT()
struct FJolfPawnFlybySound
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	class USoundBase* Sound;

	/** Minimum difference in speed between pawn and observer before this sound can play. */
	UPROPERTY(EditAnywhere)
	float MinSpeed;
};

USTRUCT()
struct FJolfPawnRepScore
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<class AJolfHole> WeakOverlappingHole;

	UPROPERTY()
	int32 NumStrokes;

	UPROPERTY()
	float Time;
};

UCLASS()
class JOLF_API AJolfPawn : public APawn
{
	GENERATED_BODY()

public:

	AJolfPawn(const FObjectInitializer& ObjectInitializer);

public: // Functions

	class USphereComponent* GetCollisionComponent() const { return CollisionComponent; }
	class UJolfMovementComponent* GetJolfMovementComponent() const { return MovementComponent; }
	class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	class AJolfPowerArrow* GetPowerArrow() const { return ArrowInstance; }

	bool IsLaunching() const { return bLaunching; }
	float GetInputStrength() const { return InputStrength; }
	float GetInputPitch() const { return InputPitch; }

	void OnMovementImpact(const FHitResult& InHitResult);
	void OnHoleOverlap(class AJolfHole* InHole);
	TWeakObjectPtr<class AJolfHole> GetOverlappingHole() { return RepScore.WeakOverlappingHole; }
	const FJolfPawnRepScore& GetScore() const { return RepScore; }

	void RecordClientSimulationResult();
	bool HasMovedYet() const { return RepMove.MoveId > 0; }
	float GetIdleTime() const;

	/** Called on authority by game mode. */
	void OnEnablePawnCollisionChanged(bool bInEnablePawnCollision);

	bool CanFakeInput() const { return CanLaunch(); }
	void FakeInput(const FRotator& InControlRotation, float InPower);

public: // Interfaces

	//~ Begin UObject Interface
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin AActor Interface
	FVector GetVelocity() const override;
	void PreInitializeComponents() override;
	void Tick(float DeltaSeconds) override;
	void BecomeViewTarget(class APlayerController* PC) override;
	void EndViewTarget(class APlayerController* PC) override;
	void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
	void FellOutOfWorld(const class UDamageType& dmgType) override;
	void DisplayDebug(class UCanvas* Canvas, const class FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

public: // Properties

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Move)
	FJolfPawnRepMove RepMove;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_ServerSimulationResult)
	FJolfPawnAuthState ServerSimulationResult;

	UPROPERTY(Transient)
	FJolfPawnAuthState ServerSimulationFutureResult;

	/**
	 * Client records location and velocity result after every simulation tick. If a server simulation result arrives
	 * for a past tick the client can compare whether its simulation was correct, and resimulate if necessary.
	 */
	UPROPERTY(Transient)
	TArray<FJolfPawnHistory> ClientSimulationHistory;

	/**
	 * Set to true after moving in sequential turns mode to signal to gamemode that it can change the active player once this pawn
	 * is idle again.
	 */
	UPROPERTY(Transient)
	uint8 bUsedSequentialTurnMove:1;
	
	/** Has hole completion timer fired? */
	UPROPERTY(Transient, Replicated)
	uint8 bWantsToFinishHole:1;

	/** Is client allowed to submit a move? */
	UPROPERTY(Transient, Replicated)
	uint8 bCanMove:1;

protected: // Interfaces

	//~ Begin APawn Interface
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	//~ End APawn Interface

private: // Functions

	UFUNCTION(Server, Reliable)
	void ServerMove(const FRotator& InControlRotation, float InPower, float InPitch);

	/** Apply the top mulligan without considering game rules. */
	void ApplyMulligan();

	UFUNCTION(Server, Reliable)
	void ServerMulligan();

	UFUNCTION()
	void OnRep_Score();

	UFUNCTION()
	void OnRep_Move();

	UFUNCTION()
	void OnRep_ServerSimulationResult();

	UFUNCTION()
	void OnRep_PawnCollisionResponse();

	void OnLaunchPressed();
	void OnCancelPressed();
	void OnMulliganPressed();
	void OnStrokePitchPressed();
	void OnStrokePitchReleased();

	void OnPitchInput(float InAxisValue);
	void OnYawInput(float InAxisValue);
	void OnMouseStrengthInput(float InAxisValue);
	void OnTurnPitchAtRateInput(float InAxisValue);
	void OnTurnYawAtRateInput(float InAxisValue);
	void OnGamepadStrengthInput(float InAxisValue);
	void OnZoomAxisInput(float InAxisValue);

	void OnHoleCompletionTimerFired();
	void SynchronizeSpringArmZoom();
	FVector GetControlDirection(const FRotator& InControlRotation, float InStrokePitch) const;

	bool CanLaunch() const;
	void SetIsLaunching(bool bNewLaunching);

	void TickFlybyAudio(float InDeltaSeconds);

private: // Properties

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleDefaultsOnly)
	class UJolfMovementComponent* MovementComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleInstanceOnly, Transient)
	class UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(VisibleInstanceOnly, Transient)
	TMap<class UJolfPhysicalMaterial*, class UNiagaraComponent*> GroundTrailNiagaraComponents;

	UPROPERTY(EditAnywhere, Category = "Jolf|Squash and Stretch")
	FFloatRK4SpringInterpolator SquashAndStretchSpring;

	/** Maps impact speed to squash and stretch magnitude. */
	UPROPERTY(EditAnywhere, Category = "Jolf|Squash and Stretch")
	FRuntimeFloatCurve SquashAndStretchImpactSpeedCurve;

	UPROPERTY(Transient)
	FVector SquashAndStretchDirection;

	/** Maps speed to smear scale. */
	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve SpeedSmearCurve;

	UPROPERTY(VisibleDefaultsOnly)
	class UNiagaraComponent* SpeedNiagaraComponent;
	
	/** Plays "whoosh" sound when passing local camera at high speed. */
	UPROPERTY(VisibleDefaultsOnly)
	class UAudioComponent* FlybyAudioComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleDefaultsOnly)
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class UNiagaraComponent* SceneProbeBrushComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AJolfProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Zoom", Meta = (ClampMin = 0, ClampMax = 1))
	float NormalizedZoom;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	FFloatInterval SpringArmZoomLength;

	UPROPERTY(EditAnywhere, Category = "Zoom")
	FFloatInterval SpringArmZoomOffset;

	UPROPERTY(ReplicatedUsing = OnRep_Score)
	FJolfPawnRepScore RepScore;

	UPROPERTY()
	float InputStrength;

	UPROPERTY()
	float InputPitch;

	UPROPERTY()
	uint8 bLaunching:1;

	UPROPERTY()
	uint8 bStrokePitchHeld:1;

	UPROPERTY(Transient)
	uint8 bHasInitialServerState:1;

	UPROPERTY(Transient)
	uint8 bShouldTickPawnCollisionResponse:1;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_PawnCollisionResponse)
	uint8 PawnCollisionResponse;

	UPROPERTY()
	TMap<class APlayerController*, class UUserWidget*> PawnWidgetInstanceMap;

	UPROPERTY(VisibleInstanceOnly, Transient)
	class AJolfProjectile* Projectile;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HoleScoreClass;
	
	UPROPERTY()
	TMap<class APlayerController*, class UUserWidget*> HoleScoreInstanceMap;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> PawnWidgetClass;

	UPROPERTY()
	TMap<class APlayerController*, class UUserWidget*> SpectateWidgetInstanceMap;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> SpectateWidgetWidgetClass;

	UPROPERTY()
	FTimerHandle HoleCompletionTimer;

	UPROPERTY(VisibleInstanceOnly)
	TArray<FJolfPawnMulliganState> MulliganStack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AJolfPowerArrow> ArrowClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<class USoundBase*> StrokeSounds;

	UPROPERTY(EditDefaultsOnly)
	class USoundBase* GoalSound;
	
	/**
	 * FlybyAudioComponent plays these "whoosh" sounds when passing local camera at high speed.
	 * Sorted from low to high min speed.
	 */
	UPROPERTY(EditDefaultsOnly)
	TArray<FJolfPawnFlybySound> FlybySounds;

	UPROPERTY(Transient)
	class AJolfPowerArrow* ArrowInstance;

	UPROPERTY(Transient)
	class AJolfPowerArrow* ShadowArrowInstance;

	UPROPERTY(Transient)
	float FlybyCooldown;

	/** Seconds as the active player since spawn. */
	UPROPERTY(Transient)
	float PlayTime;

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	FTimerHandle DebugServerMoveTimer;
#endif !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
};