// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSequentialTurnsEnabledTest.h"
#include "JolfGameMode.h"
#include "JolfGameState.h"
#include "JolfPlayerController.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"

AJolfSequentialTurnsEnabledTest::AJolfSequentialTurnsEnabledTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Only the active pawn should be able to move and be view target when SequentialTurns is enabled.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfSequentialTurnsEnabledTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnableSequentialTurns(true);

	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertTrue(GameState->bSequentialTurns, TEXT("sequential turns is enabled"));

	RestartAllPlayers();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfSequentialTurnsEnabledTest::LaunchPawns, 1.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfSequentialTurnsEnabledTest::LaunchPawns()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	AssertIsValid(GameState->ActivePlayerState, TEXT("has active player"));

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	for (AJolfPlayerController* PC : PlayerControllers)
	{
		AJolfPawn* Pawn = PC->GetPawn<AJolfPawn>();
		if (AssertIsValid(Pawn, TEXT("player has pawn"), PC))
		{
			AssertTrue(Pawn->GetIdleTime() > 0.1f, TEXT("pawn is idle"), Pawn);
			AssertEqual_Object(PC->GetViewTarget(), GameState->ActivePlayerState->GetPawn(), TEXT("player is viewing active player"), PC);
			
			// Fake input, but only active should move.
			Pawn->FakeInput(FRotator::ZeroRotator, 0.25f);
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfSequentialTurnsEnabledTest::TestPawnsMoved, 0.1f);
}

void AJolfSequentialTurnsEnabledTest::TestPawnsMoved()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	for (AJolfPlayerController* PC : PlayerControllers)
	{
		AJolfPawn* Pawn = PC->GetPawn<AJolfPawn>();
		if (AssertIsValid(Pawn, TEXT("player has pawn"), PC))
		{
			// Only the active pawn should have moved.
			if (PC->PlayerState == GameState->ActivePlayerState)
			{
				AssertTrue(Pawn->GetIdleTime() < 0.1f, TEXT("active pawn is moving"), Pawn);
			}
			else
			{
				AssertTrue(Pawn->GetIdleTime() > 0.1f, TEXT("non-active pawn is idle"), Pawn);
			}
		}
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}