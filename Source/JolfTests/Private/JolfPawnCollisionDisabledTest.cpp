// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPawnCollisionDisabledTest.h"
#include "JolfGameMode.h"
#include "JolfGameState.h"
#include "JolfPlayerController.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"

AJolfPawnCollisionDisabledTest::AJolfPawnCollisionDisabledTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Pawns should move through each other when collision is disabled.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfPawnCollisionDisabledTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnablePawnCollision(false);

	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertFalse(GameState->bEnablePawnCollision, TEXT("pawn collision is disabled"));

	RestartAllPlayers();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfPawnCollisionDisabledTest::PawnsOverlapAtSpawn, 1.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfPawnCollisionDisabledTest::PawnsOverlapAtSpawn()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		AssertEqual_Vector(Pawns[0]->GetActorLocation(), Pawns[1]->GetActorLocation(), TEXT("overlapping spawn location"), 0.1f);

		Pawns[1]->TeleportTo(Pawns[0]->GetActorLocation() + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfPawnCollisionDisabledTest::PawnsOverlapAfterMovement, 1.0f);
}

void AJolfPawnCollisionDisabledTest::PawnsOverlapAfterMovement()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		float DistanceSquared = (Pawns[0]->GetActorLocation() - Pawns[1]->GetActorLocation()).SizeSquared();
		AssertTrue(DistanceSquared < 4.f, TEXT("overlapping after movement"));
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}