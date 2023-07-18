// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfFlagstick.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "JolfPawn.h"

AJolfFlagstick::AJolfFlagstick(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	TriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	TriggerComponent->SetGenerateOverlapEvents(true);
	TriggerComponent->InitSphereRadius(300.f);
	RootComponent = TriggerComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(TriggerComponent);

	LiftSpring.SetDefaultSpringConstants(100.f, 0.4f);
	LiftTargetDistance = 100.f;
}

// Public Interfaces:

//~ Begin AActor Interface
void AJolfFlagstick::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (AJolfPawn* Pawn = Cast<AJolfPawn>(OtherActor))
	{
		int32 Index = OverlappingPawns.AddUnique(Pawn);
		if (OverlappingPawns.Num() == 1 && Index != INDEX_NONE)
		{
			SetActorTickEnabled(true);
		}
	}
}

void AJolfFlagstick::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
	
	if (AJolfPawn* Pawn = Cast<AJolfPawn>(OtherActor))
	{
		OverlappingPawns.RemoveSingleSwap(Pawn);
	}
}

void AJolfFlagstick::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Only lift if there are pawns not yet in the hole.
	bool bAnyPawns = false;
	for (AJolfPawn* Pawn : OverlappingPawns)
	{
		if (Pawn && !Pawn->GetOverlappingHole().IsValid())
		{
			bAnyPawns = true;
			break;
		}
	}
	
	float LiftCurrentDistance = LiftSpring.Update(bAnyPawns ? LiftTargetDistance : 0.f, DeltaSeconds);
	MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, LiftCurrentDistance));

	if (OverlappingPawns.Num() < 1 && FMath::IsNearlyZero(LiftCurrentDistance))
	{
		SetActorTickEnabled(false);
	}
}
//~ End AActor Interface