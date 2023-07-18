// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfWaitForAllPlayersDisabledTest.h"
#include "JolfGameMode.h"
#include "JolfPlayerController.h"
#include "JolfGameState.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"
#include "JolfHole.h"

AJolfWaitForAllPlayersDisabledTest::AJolfWaitForAllPlayersDisabledTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Landing in the hole with WaitForAllPlayers disabled should allow the player to continue to the next hole.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfWaitForAllPlayersDisabledTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetWaitForAllPlayers(false);

	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertFalse(GameState->bShouldWaitForAllPlayersToFinishHole, TEXT("wait for all players is disabled"));

	RestartAllPlayers();

	AssertTrue(GameState->Holes.Num() > 0, TEXT("has holes"));

	AJolfPawn* Pawn = PlayerControllers[0]->GetPawn<AJolfPawn>();
	if (AssertIsValid(Pawn, TEXT("pawn exists")))
	{
		Pawn->TeleportTo(GameState->Holes[0]->GetActorLocation() + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfWaitForAllPlayersDisabledTest::TestPlayerDidNotWait, 5.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfWaitForAllPlayersDisabledTest::TestPlayerDidNotWait()
{
	TArray<AJolfPlayerState*> PlayerStates = GetPlayerStates();
	if (AssertEqual_Int(PlayerStates.Num(), 2, TEXT("num player states")))
	{
		AssertTrue(PlayerStates[0]->HoleIndex == 1, TEXT("first player continued to next hole"), PlayerStates[0]);
		AssertTrue(PlayerStates[1]->HoleIndex == 0, TEXT("second player remained at first hole"), PlayerStates[1]);
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}
