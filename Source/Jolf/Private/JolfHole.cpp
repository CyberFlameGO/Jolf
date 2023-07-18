// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfHole.h"
#include "Components/StaticMeshComponent.h"
#include "JolfPawn.h"
#include "JolfPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "JolfGameState.h"

#if WITH_EDITOR
#include "EngineUtils.h"
#include "JolfPlayerStart.h"
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#include "Logging/TokenizedMessage.h"
#endif // WITH_EDITOR

AJolfHole::AJolfHole(const FObjectInitializer& ObjectInitializer)
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	TriggerComponent->SetupAttachment(MeshComponent);
	TriggerComponent->SetGenerateOverlapEvents(true);

	Par = 1;
}

// Public Interfaces:

//~ Begin AActor Interface
void AJolfHole::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
	{
		GameState->Holes.AddUnique(this);
	}
}

void AJolfHole::Destroyed()
{
	if (AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
	{
		GameState->Holes.RemoveSingleSwap(this);
	}

	Super::Destroyed();
}

void AJolfHole::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!HasAuthority())
	{
		// Prevent mispredicting goal.
		return;
	}

	if (AJolfPawn* Pawn = Cast<AJolfPawn>(OtherActor))
	{
		if (AJolfPlayerState* State = Pawn->GetPlayerState<AJolfPlayerState>())
		{
			if(State->HoleIndex == HoleIndex)
			{
				Pawn->OnHoleOverlap(this);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Wrong hole %s"), *GetNameSafe(Pawn));
			}
		}
	}
}

#if WITH_EDITOR
void AJolfHole::CheckForErrors()
{
	Super::CheckForErrors();

	AJolfPlayerStart* PairedPlayerStart = nullptr;
	for (TActorIterator<AJolfPlayerStart> Iter(GetWorld()); Iter; ++Iter)
	{
		if (Iter->HoleIndex == HoleIndex)
		{
			PairedPlayerStart = *Iter;
			break;
		}
	}

	if (PairedPlayerStart == nullptr)
	{
		FMessageLog("MapCheck").Error()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(NSLOCTEXT("Jolf", "MapCheck_HoleWithoutPlayerStart", "This hole does not have a paired player start.")));
	}
}
#endif // WITH_EDITOR
//~ End AActor Interface