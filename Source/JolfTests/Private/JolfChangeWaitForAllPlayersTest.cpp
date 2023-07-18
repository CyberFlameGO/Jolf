// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfChangeWaitForAllPlayersTest.h"
#include "JolfPlayerController.h"
#include "JolfGameMode.h"
#include "JolfGameState.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"
#include "JolfHole.h"

AJolfChangeWaitForAllPlayersTest::AJolfChangeWaitForAllPlayersTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Changing WaitForAllPlayers should cause players further ahead to spectate players further behind.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfChangeWaitForAllPlayersTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetWaitForAllPlayers(true);

	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertTrue(GameState->bShouldWaitForAllPlayersToFinishHole, TEXT("wait for all players is enabled by default"));

	RestartAllPlayers();

	AssertTrue(GameState->Holes.Num() > 0, TEXT("has holes"));

	AJolfPawn* Pawn = PlayerControllers[0]->GetPawn<AJolfPawn>();
	if (AssertIsValid(Pawn, TEXT("pawn exists")))
	{
		Pawn->TeleportTo(GameState->Holes[0]->GetActorLocation() + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfChangeWaitForAllPlayersTest::TestPlayerWaited, 5.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfChangeWaitForAllPlayersTest::TestPlayerWaited()
{
	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers"));

	if (AJolfPlayerState* WaitingPS = PlayerControllers[0]->GetPlayerState<AJolfPlayerState>())
	{
		AssertTrue(WaitingPS->HoleIndex == 0, TEXT("player in hole waited for other player"));
	}

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetWaitForAllPlayers(false);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfChangeWaitForAllPlayersTest::DisabledWaitForAllPlayers, 1.0f);
}

void AJolfChangeWaitForAllPlayersTest::DisabledWaitForAllPlayers()
{
	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertFalse(GameState->bShouldWaitForAllPlayersToFinishHole, TEXT("changed wait for all players to disabled"));

	TArray<AJolfPlayerState*> PlayerStates = GetPlayerStates();
	if (AssertEqual_Int(PlayerStates.Num(), 2, TEXT("num player states")))
	{
		AssertTrue(PlayerStates[0]->HoleIndex == 1, TEXT("first player continued to next hole"), PlayerStates[0]);
		AssertTrue(PlayerStates[1]->HoleIndex == 0, TEXT("second player remained at first hole"), PlayerStates[1]);
	}

	GameMode->SetWaitForAllPlayers(true);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfChangeWaitForAllPlayersTest::EnabledWaitForAllPlayers, 1.0f);
}

void AJolfChangeWaitForAllPlayersTest::EnabledWaitForAllPlayers()
{
	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertTrue(GameState->bShouldWaitForAllPlayersToFinishHole, TEXT("changed wait for all players to enabled"));

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	if (AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers")))
	{
		AssertEqual_Object(PlayerControllers[0]->GetViewTarget(), PlayerControllers[1]->GetPawn(), TEXT("first player viewing second player after enabling wait for all players"));
	}

	GameMode->SetWaitForAllPlayers(false);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfChangeWaitForAllPlayersTest::DisabledWaitForAllPlayersAgain, 1.0f);
}

void AJolfChangeWaitForAllPlayersTest::DisabledWaitForAllPlayersAgain()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	AssertFalse(GameState->bShouldWaitForAllPlayersToFinishHole, TEXT("changed wait for all players to disabled again"));

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	if (AssertEqual_Int(PlayerControllers.Num(), 2, TEXT("num player controllers")))
	{
		AssertEqual_Object(PlayerControllers[0]->GetViewTarget(), PlayerControllers[0]->GetPawn(), TEXT("first player viewing self after disabling wait for all players"));
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}