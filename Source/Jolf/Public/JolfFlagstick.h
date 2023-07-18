// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/Actor.h"
#include "Engine/SpringInterpolator.h"
#include "JolfFlagstick.generated.h"

UCLASS()
class AJolfFlagstick : public AActor
{
	GENERATED_BODY()

public:

	AJolfFlagstick(const FObjectInitializer& ObjectInitializer);

public: // Interfaces

	//~ Begin AActor Interface
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyActorEndOverlap(AActor* OtherActor) override;
	void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

private: // Properties

	UPROPERTY(Transient)
	TArray<class AJolfPawn*> OverlappingPawns;

	UPROPERTY(EditAnywhere, Category = "Flagstick")
	FFloatRK4SpringInterpolator LiftSpring;

	UPROPERTY(EditAnywhere, Category = "Flagstick")
	float LiftTargetDistance;

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* TriggerComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* MeshComponent;
};