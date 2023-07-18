// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfEnablePawnCollisionDuringOverlapTest.h"
#include "JolfGameMode.h"
#include "JolfGameState.h"
#include "JolfPlayerController.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"

AJolfEnablePawnCollisionDuringOverlapTest::AJolfEnablePawnCollisionDuringOverlapTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Enabling pawn collision during overlap should not block until finished overlapping.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfEnablePawnCollisionDuringOverlapTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnablePawnCollision(false);

	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertFalse(GameState->bEnablePawnCollision, TEXT("pawn collision is disabled"));

	RestartAllPlayers();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfEnablePawnCollisionDuringOverlapTest::PawnsOverlapWhileDisabled, 1.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfEnablePawnCollisionDuringOverlapTest::PawnsOverlapWhileDisabled()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		float DistanceSquared = (Pawns[0]->GetActorLocation() - Pawns[1]->GetActorLocation()).SizeSquared();
		AssertTrue(DistanceSquared < 4.f, TEXT("overlapping while disabled"));
	}

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnablePawnCollision(true);
	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertTrue(GameState->bEnablePawnCollision, TEXT("pawn collision is enabled"));

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfEnablePawnCollisionDuringOverlapTest::PawnsOverlapAfterEnabling, 1.0f);
}

void AJolfEnablePawnCollisionDuringOverlapTest::PawnsOverlapAfterEnabling()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		float DistanceSquared = (Pawns[0]->GetActorLocation() - Pawns[1]->GetActorLocation()).SizeSquared();
		AssertTrue(DistanceSquared < 4.f, TEXT("overlapping after enabling"));

		Pawns[1]->TeleportTo(Pawns[0]->GetActorLocation() + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfEnablePawnCollisionDuringOverlapTest::PawnsBlockAfterMovement, 1.0f);
}

void AJolfEnablePawnCollisionDuringOverlapTest::PawnsBlockAfterMovement()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		float DistanceSquared = (Pawns[0]->GetActorLocation() - Pawns[1]->GetActorLocation()).SizeSquared();
		AssertTrue(DistanceSquared > 400.f, TEXT("blocked after movement"));
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}
