// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSequentialTurnsDisabledTest.h"
#include "JolfGameMode.h"
#include "JolfGameState.h"
#include "JolfPlayerController.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"

AJolfSequentialTurnsDisabledTest::AJolfSequentialTurnsDisabledTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("All pawns should be able to move simultaneously while SequentialTurns is disabled.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfSequentialTurnsDisabledTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnableSequentialTurns(false);

	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertFalse(GameState->bSequentialTurns, TEXT("sequential turns is disabled"));

	RestartAllPlayers();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfSequentialTurnsDisabledTest::LaunchPawns, 1.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfSequentialTurnsDisabledTest::LaunchPawns()
{
	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	for (AJolfPlayerController* PC : PlayerControllers)
	{
		AJolfPawn* Pawn = PC->GetPawn<AJolfPawn>();
		if (AssertIsValid(Pawn, TEXT("player has pawn"), PC))
		{
			AssertTrue(Pawn->GetIdleTime() > 0.1f, TEXT("pawn is idle"), Pawn);
			AssertEqual_Object(PC->GetViewTarget(), Pawn, TEXT("player is viewing their own pawn"), PC);
			Pawn->FakeInput(FRotator::ZeroRotator, 0.25f);
		}
	}
	
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfSequentialTurnsDisabledTest::TestPawnsMoved, 0.1f);
}

void AJolfSequentialTurnsDisabledTest::TestPawnsMoved()
{
	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	for (AJolfPlayerController* PC : PlayerControllers)
	{
		AJolfPawn* Pawn = PC->GetPawn<AJolfPawn>();
		if (AssertIsValid(Pawn, TEXT("player has pawn"), PC))
		{
			AssertTrue(Pawn->GetIdleTime() < 0.1f, TEXT("pawn is moving"), Pawn);
		}
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}