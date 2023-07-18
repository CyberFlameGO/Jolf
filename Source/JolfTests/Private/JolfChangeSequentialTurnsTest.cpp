// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfChangeSequentialTurnsTest.h"
#include "JolfGameMode.h"
#include "JolfPlayerController.h"
#include "JolfGameState.h"
#include "JolfPlayerState.h"

AJolfChangeSequentialTurnsTest::AJolfChangeSequentialTurnsTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Disabling sequential turns should cause players to view their own pawn, and enabling should cause players to view the active pawn.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfChangeSequentialTurnsTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnableSequentialTurns(true);

	RestartAllPlayers();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfChangeSequentialTurnsTest::TestDefaultSequentialTurnsEnabled, 1.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfChangeSequentialTurnsTest::TestDefaultSequentialTurnsEnabled()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	AssertIsValid(GameState->ActivePlayerState, TEXT("has active player"));
	AssertTrue(GameState->bSequentialTurns, TEXT("in sequential turns mode"));

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		AssertEqual_Object((*Iter)->GetViewTarget(), GameState->ActivePlayerState->GetPawn(), TEXT("player view target active"), Iter->Get());
	}

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnableSequentialTurns(false);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfChangeSequentialTurnsTest::TestSequentialTurnsDisabled, 1.0f);
}

void AJolfChangeSequentialTurnsTest::TestSequentialTurnsDisabled()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	AssertFalse(GameState->bSequentialTurns, TEXT("not in sequential turns mode after disabling sequential turns"));

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		AssertEqual_Object((*Iter)->GetViewTarget(), (*Iter)->PlayerState->GetPawn(), TEXT("player view target self after disabling sequential turns"), Iter->Get());
	}

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnableSequentialTurns(true);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfChangeSequentialTurnsTest::TestSequentialTurnsEnabled, 1.0f);
}

void AJolfChangeSequentialTurnsTest::TestSequentialTurnsEnabled()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	AssertIsValid(GameState->ActivePlayerState, TEXT("has active player after enabling sequential turns"));
	AssertTrue(GameState->bSequentialTurns, TEXT("in sequential turns mode after enabling sequential turns"));

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		AssertEqual_Object((*Iter)->GetViewTarget(), GameState->ActivePlayerState->GetPawn(), TEXT("player view target active after enabling sequential turns"), Iter->Get());
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}