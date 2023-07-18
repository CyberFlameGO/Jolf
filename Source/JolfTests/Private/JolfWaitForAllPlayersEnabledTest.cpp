// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfWaitForAllPlayersEnabledTest.h"
#include "JolfGameMode.h"
#include "JolfPlayerController.h"
#include "JolfGameState.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"
#include "JolfHole.h"

AJolfWaitForAllPlayersEnabledTest::AJolfWaitForAllPlayersEnabledTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Landing in the hole with WaitForAllPlayers enabled should cause the player to spectate until both players proceed to the next hole.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfWaitForAllPlayersEnabledTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetWaitForAllPlayers(true);

	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertTrue(GameState->bShouldWaitForAllPlayersToFinishHole, TEXT("wait for all players is enabled"));

	RestartAllPlayers();

	AssertTrue(GameState->Holes.Num() > 0, TEXT("has holes"));

	AJolfPawn* Pawn = PlayerControllers[0]->GetPawn<AJolfPawn>();
	if (AssertIsValid(Pawn, TEXT("pawn exists")))
	{
		Pawn->TeleportTo(GameState->Holes[0]->GetActorLocation() + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfWaitForAllPlayersEnabledTest::TestPlayerWaited, 5.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfWaitForAllPlayersEnabledTest::TestPlayerWaited()
{
	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	if (AJolfPlayerState* WaitingPS = PlayerControllers[0]->GetPlayerState<AJolfPlayerState>())
	{
		AssertTrue(WaitingPS->HoleIndex == 0, TEXT("player in hole waited for other player"));
	}

	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	AJolfPawn* Pawn = PlayerControllers[1]->GetPawn<AJolfPawn>();
	if (AssertIsValid(Pawn, TEXT("second pawn exists")))
	{
		Pawn->TeleportTo(GameState->Holes[0]->GetActorLocation() + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfWaitForAllPlayersEnabledTest::TestPlayersContinued, 5.0f);
}

void AJolfWaitForAllPlayersEnabledTest::TestPlayersContinued()
{
	TArray<AJolfPlayerState*> PlayerStates = GetPlayerStates();
	AssertEqual_Int(PlayerStates.Num(), 2, TEXT("num player states"));

	for (AJolfPlayerState* PS : PlayerStates)
	{
		AssertTrue(PS->HoleIndex == 1, TEXT("player continued to next hole"), PS);
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}