// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfGameMode.h"
#include "JolfPlayerState.h"
#include "JolfPlayerStart.h"
#include "EngineUtils.h"
#include "JolfPlayerController.h"
#include "JolfGameState.h"
#include "Kismet/GameplayStatics.h"
#include "JolfPawn.h"
#include "JolfLevelDefinition.h"

AJolfGameMode::AJolfGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	GameStateClass = AJolfGameState::StaticClass();
	PlayerStateClass = AJolfPlayerState::StaticClass();
	PlayerControllerClass = AJolfPlayerController::StaticClass();

	bUseSeamlessTravel = true;
}

// Public Functions:

void AJolfGameMode::SetEnablePawnCollision(bool bInEnablePawnCollision)
{
	AJolfGameState* JolfGameState = GetGameState<AJolfGameState>();
	if (!ensure(JolfGameState))
		return;

	if (JolfGameState->bEnablePawnCollision == bInEnablePawnCollision)
		return;

	for (TActorIterator<AJolfPawn> Iter(GetWorld()); Iter; ++Iter)
	{
		AJolfPawn* Pawn = *Iter;
		check(Pawn);
		Pawn->OnEnablePawnCollisionChanged(bInEnablePawnCollision);
	}

	JolfGameState->SetEnablePawnCollision(bInEnablePawnCollision);
}

void AJolfGameMode::SetEnableSequentialTurns(bool bInEnableSequentialTurns)
{
	AJolfGameState* JolfGameState = GetGameState<AJolfGameState>();
	if (!ensure(JolfGameState))
		return;

	if (JolfGameState->bSequentialTurns == bInEnableSequentialTurns)
		return;

	JolfGameState->SetSequentialTurns(bInEnableSequentialTurns);

	// Mark all pawns as not having used their turn yet.
	for (TActorIterator<AJolfPawn> Iter(GetWorld()); Iter; ++Iter)
	{
		AJolfPawn* Pawn = *Iter;
		check(Pawn);
		Pawn->bUsedSequentialTurnMove = false;
	}
}

void AJolfGameMode::SetWaitForAllPlayers(bool bInWaitForAllPlayers)
{
	AJolfGameState* JolfGameState = GetGameState<AJolfGameState>();
	if (!ensure(JolfGameState))
		return;

	if (JolfGameState->bShouldWaitForAllPlayersToFinishHole == bInWaitForAllPlayers)
		return;

	JolfGameState->SetWaitForAllPlayers(bInWaitForAllPlayers);

	if (bInWaitForAllPlayers)
	{
		// Anyone ahead of the furthest back player should switch to spectating.

		int32 MinHoleIndex = MAX_int32;
		for (APlayerState* PS : JolfGameState->PlayerArray)
		{
			if (AJolfPlayerState* TypedPS = Cast<AJolfPlayerState>(PS))
			{
				MinHoleIndex = FMath::Min(MinHoleIndex, TypedPS->HoleIndex);
			}
		}

		for (APlayerState* PS : JolfGameState->PlayerArray)
		{
			if (AJolfPlayerState* TypedPS = Cast<AJolfPlayerState>(PS))
			{
				if (TypedPS->HoleIndex > MinHoleIndex)
				{
					if (APawn* Pawn = TypedPS->GetPawn())
					{
						Pawn->Destroy();
					}

					AJolfPlayerController* PlayerController = Cast<AJolfPlayerController>(PS->GetOwner());
					if (ensure(PlayerController))
					{
						PlayerController->ViewAPlayer(+1);
					}
				}
			}
		}
	}
	else
	{
		// Everyone who was waiting can respawn and continue playing.

		for (APlayerState* PS : JolfGameState->PlayerArray)
		{
			if (AJolfPlayerState* TypedPS = Cast<AJolfPlayerState>(PS))
			{
				AJolfPlayerController* PC = Cast<AJolfPlayerController>(PS->GetOwner());
				if (!ensure(PC))
					return;

				AJolfPawn* Pawn = PS->GetPawn<AJolfPawn>();
				if (Pawn)
				{
					if (Pawn->bWantsToFinishHole)
					{
						++TypedPS->HoleIndex;
						TypedPS->StrokeIndex = 0;
						TypedPS->NumMulligansUsed = 0;

						PC->StartSpot.Reset();
						Pawn->Destroy();
						RestartPlayer(PC);
					}
				}
				else
				{
					// Spectating without pawn.
					TypedPS->StrokeIndex = 0;
					TypedPS->NumMulligansUsed = 0;

					PC->StartSpot.Reset();
					RestartPlayer(PC);
				}
			}
		}
	}
}

void AJolfGameMode::SetAllowStrokeWhileMoving(bool bInAllowStrokeWhileMoving)
{
	AJolfGameState* JolfGameState = GetGameState<AJolfGameState>();
	if (!ensure(JolfGameState))
		return;

	if (JolfGameState->bAllowStrokeWhileMoving == bInAllowStrokeWhileMoving)
		return;

	JolfGameState->SetAllowStrokeWhileMoving(bInAllowStrokeWhileMoving);
}

void AJolfGameMode::RequestTravel(const UJolfLevelDefinition& InLevelDef)
{
	AJolfGameState* JolfGameState = GetGameState<AJolfGameState>();
	if (!ensure(JolfGameState))
		return;

	UWorld* World = GetWorld();
	check(World);

	FString NewURL = FString::Printf(TEXT("%s?Collision=%i?UseActivePlayer=%i?WaitForAllPlayers=%i"),
		*InLevelDef.Level.GetLongPackageName(),
		JolfGameState->bEnablePawnCollision,
		JolfGameState->bSequentialTurns,
		JolfGameState->bShouldWaitForAllPlayersToFinishHole
		);

#if WITH_EDITOR
	if (World->IsPlayInEditor())
	{
		// PIE seamless travel is not supported by Unreal yet.
		bUseSeamlessTravel = false;
	}
#endif // WITH_EDITOR

	World->ServerTravel(NewURL, /*bAbsolute*/ true);
}

// Public Interfaces:

//~ Begin AActor Interface
void AJolfGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	AJolfGameState* JolfGameState = GetGameState<AJolfGameState>();
	if (JolfGameState == nullptr)
		return;

	if (JolfGameState->ActivePlayerState == nullptr)
	{
		JolfGameState->SetActivePlayerState(GetNextActivePlayerState());
	}
	else
	{
		if (AJolfPawn* Pawn = JolfGameState->ActivePlayerState->GetPawn<AJolfPawn>())
		{
			if (Pawn->bUsedSequentialTurnMove && Pawn->GetIdleTime() > 0.6f)
			{
				JolfGameState->SetActivePlayerState(GetNextActivePlayerState());
			}
		}
	}

	if (JolfGameState->bShouldWaitForAllPlayersToFinishHole)
	{
		bool bWaitingForAnyPawns = false;
		for (TActorIterator<AJolfPlayerController> PCIter(GetWorld()); PCIter; ++PCIter)
		{
			AJolfPlayerController* PC = *PCIter;
			check(PC);
			AJolfPawn* Pawn = PC->GetPawn<AJolfPawn>();
			if (Pawn && !Pawn->bWantsToFinishHole)
			{
				bWaitingForAnyPawns = true;
				break;
			}
		}

		if (!bWaitingForAnyPawns)
		{
			for (TActorIterator<AJolfPlayerController> PCIter(GetWorld()); PCIter; ++PCIter)
			{
				AJolfPlayerController* PC = *PCIter;
				check(PC);

				AJolfPawn* Pawn = PC->GetPawn<AJolfPawn>();
				AJolfPlayerState* PlayerState = PC->GetPlayerState<AJolfPlayerState>();
				if (Pawn && PlayerState)
				{
					++PlayerState->HoleIndex;
					PlayerState->StrokeIndex = 0;
					PlayerState->NumMulligansUsed = 0;

					PC->StartSpot.Reset();
					Pawn->Destroy();
					RestartPlayer(PC);
				}
			}
		}
	}
}
//~ End AActor Interface

//~ Begin AGameModeBase Interface
AActor* AJolfGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AJolfPlayerState* PlayerState = Player->GetPlayerState<AJolfPlayerState>();
	if(PlayerState)
	{
		for (TActorIterator<AJolfPlayerStart> Iter(GetWorld()); Iter; ++Iter)
		{
			if (Iter->HoleIndex == PlayerState->HoleIndex)
			{
				return *Iter;
			}
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);	
}

void AJolfGameMode::InitGameState()
{
	Super::InitGameState();
	
	if (AJolfGameState* JolfGameState = GetGameState<AJolfGameState>())
	{
		JolfGameState->bEnablePawnCollision = UGameplayStatics::ParseOption(OptionsString, TEXT("Collision")) == TEXT("1");
		JolfGameState->bSequentialTurns = UGameplayStatics::ParseOption(OptionsString, TEXT("Hotseat")) == TEXT("1")
			|| UGameplayStatics::ParseOption(OptionsString, TEXT("UseActivePlayer")) == TEXT("1");

		FString WaitForAllPlayers = UGameplayStatics::ParseOption(OptionsString, TEXT("WaitForAllPlayers"));
		if (!WaitForAllPlayers.IsEmpty())
		{
			JolfGameState->bShouldWaitForAllPlayersToFinishHole = WaitForAllPlayers == TEXT("1");
		}
	}
}
//~ End AGameModeBase Interface

// Private Functions:

AJolfPlayerState* AJolfGameMode::GetNextActivePlayerState()
{
	AJolfGameState* JolfGameState = GetGameState<AJolfGameState>();
	check(JolfGameState);

	if (JolfGameState->PlayerArray.Num() < 1)
		return nullptr;

	int32 NextIndex;
	if (JolfGameState->ActivePlayerState != nullptr)
	{
		NextIndex = JolfGameState->PlayerArray.IndexOfByKey(JolfGameState->ActivePlayerState);
	}
	else
	{
		NextIndex = JolfGameState->PlayerArray.Num() - 1;
	}

	for (int32 Iteration = 0; Iteration < JolfGameState->PlayerArray.Num(); ++Iteration)
	{
		NextIndex = (NextIndex + 1) % JolfGameState->PlayerArray.Num();

		AJolfPlayerState* TestState = Cast<AJolfPlayerState>(JolfGameState->PlayerArray[NextIndex]);
		if (TestState == nullptr)
			continue;

		AJolfPawn* TestPawn = TestState->GetPawn<AJolfPawn>();
		if (TestPawn == nullptr)
			continue;

		if (!TestPawn->bWantsToFinishHole)
		{
			return TestState;
		}
	}

	return nullptr;
}