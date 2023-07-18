// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPawnCollisionEnabledTest.h"
#include "JolfGameMode.h"
#include "JolfGameState.h"
#include "JolfPlayerController.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"

AJolfPawnCollisionEnabledTest::AJolfPawnCollisionEnabledTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Pawns should block each other when collision is enabled.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfPawnCollisionEnabledTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();
	GameMode->SetEnablePawnCollision(true);

	AJolfGameState* GameState = GameMode->GetGameState<AJolfGameState>();
	AssertTrue(GameState->bEnablePawnCollision, TEXT("pawn collision is enabled"));

	RestartAllPlayers();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfPawnCollisionEnabledTest::PawnsOverlapAtSpawn, 1.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfPawnCollisionEnabledTest::PawnsOverlapAtSpawn()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		AssertEqual_Vector(Pawns[0]->GetActorLocation(), Pawns[1]->GetActorLocation(), TEXT("overlapping spawn location"), 0.1f);

		Pawns[1]->TeleportTo(Pawns[0]->GetActorLocation() + FVector(0.f, 0.f, 100.f), FRotator::ZeroRotator);
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfPawnCollisionEnabledTest::PawnsBlockAfterMovement, 1.0f);
}

void AJolfPawnCollisionEnabledTest::PawnsBlockAfterMovement()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		float DistanceSquared = (Pawns[0]->GetActorLocation() - Pawns[1]->GetActorLocation()).SizeSquared();
		AssertTrue(DistanceSquared > 400.f, TEXT("blocked after movement"));
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}