// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfHotseatTest.h"
#include "JolfViewportClient.h"
#include "JolfPawn.h"

AJolfHotseatTest::AJolfHotseatTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Input should be routed to hotseat player.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfHotseatTest::StartTest()
{
	Super::StartTest();

	SetNumLocalPlayers(2);

	ViewportClient = Cast<UJolfViewportClient>(GetWorld()->GetGameViewport());
	ViewportClient->bHotseatMode = true;
	ViewportClient->SetHotseatIndex(0);

	RestartAllPlayers();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfHotseatTest::FirstPlayerPressMouse, 1.0f);
}
//~ End AFunctionalTest Interface

// Private Functions:

void AJolfHotseatTest::FirstPlayerPressMouse()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	for (AJolfPawn* Pawn : Pawns)
	{
		AssertTrue(Pawn->GetIdleTime() > 0.1f, TEXT("pawn idle"), Pawn);
	}

	ViewportClient->InputKey(FInputKeyEventArgs(ViewportClient->Viewport, 0, EKeys::LeftMouseButton, IE_Pressed));
	GetWorldTimerManager().SetTimerForNextTick(this, &AJolfHotseatTest::FirstPlayerMoveMouse);
}

void AJolfHotseatTest::FirstPlayerMoveMouse()
{
	ViewportClient->InputKey(FInputKeyEventArgs(ViewportClient->Viewport, 0, EKeys::LeftMouseButton, IE_Released));
	ViewportClient->InputAxis(ViewportClient->Viewport, 0, EKeys::MouseY, 50.f, 1.f / 30.f, 1, false);
	GetWorldTimerManager().SetTimerForNextTick(this, &AJolfHotseatTest::FirstPlayerPressMouseAgain);
}

void AJolfHotseatTest::FirstPlayerPressMouseAgain()
{
	ViewportClient->InputKey(FInputKeyEventArgs(ViewportClient->Viewport, 0, EKeys::LeftMouseButton, IE_Pressed));
	GetWorldTimerManager().SetTimerForNextTick(this, &AJolfHotseatTest::FirstPlayerReleaseMouse);
}

void AJolfHotseatTest::FirstPlayerReleaseMouse()
{
	ViewportClient->InputKey(FInputKeyEventArgs(ViewportClient->Viewport, 0, EKeys::LeftMouseButton, IE_Released));
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfHotseatTest::AfterMoveFirstPlayer, 0.1f);
}

void AJolfHotseatTest::AfterMoveFirstPlayer()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		AssertTrue(Pawns[0]->GetVelocity().SizeSquared() > 100.f, TEXT("first pawn moving"));
		AssertTrue(Pawns[1]->GetVelocity().SizeSquared() < 100.f, TEXT("second pawn not moving"));
	}

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfHotseatTest::HotseatPlayerChanged, 5.0f);
}

void AJolfHotseatTest::HotseatPlayerChanged()
{
	AssertEqual_Int(ViewportClient->GetHotseatIndex(), 1, TEXT("second player in hotseat"));

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfHotseatTest::SecondPlayerPressMouse, 1.0f);
}

void AJolfHotseatTest::SecondPlayerPressMouse()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	for (AJolfPawn* Pawn : Pawns)
	{
		AssertTrue(Pawn->GetIdleTime() > 0.1f, TEXT("pawn idle"), Pawn);
	}

	ViewportClient->InputKey(FInputKeyEventArgs(ViewportClient->Viewport, 0, EKeys::LeftMouseButton, IE_Pressed));
	GetWorldTimerManager().SetTimerForNextTick(this, &AJolfHotseatTest::SecondPlayerMoveMouse);
}

void AJolfHotseatTest::SecondPlayerMoveMouse()
{
	ViewportClient->InputKey(FInputKeyEventArgs(ViewportClient->Viewport, 0, EKeys::LeftMouseButton, IE_Released));
	ViewportClient->InputAxis(ViewportClient->Viewport, 0, EKeys::MouseY, 50.f, 1.f / 30.f, 1, false);
	GetWorldTimerManager().SetTimerForNextTick(this, &AJolfHotseatTest::SecondPlayerPressMouseAgain);
}

void AJolfHotseatTest::SecondPlayerPressMouseAgain()
{
	ViewportClient->InputKey(FInputKeyEventArgs(ViewportClient->Viewport, 0, EKeys::LeftMouseButton, IE_Pressed));
	GetWorldTimerManager().SetTimerForNextTick(this, &AJolfHotseatTest::SecondPlayerReleaseMouse);
}

void AJolfHotseatTest::SecondPlayerReleaseMouse()
{
	ViewportClient->InputKey(FInputKeyEventArgs(ViewportClient->Viewport, 0, EKeys::LeftMouseButton, IE_Released));
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AJolfHotseatTest::AfterMoveSecondPlayer, 0.1f);
}

void AJolfHotseatTest::AfterMoveSecondPlayer()
{
	TArray<AJolfPawn*> Pawns = GetPawns();
	if (AssertEqual_Int(Pawns.Num(), 2, TEXT("num pawns")))
	{
		AssertTrue(Pawns[0]->GetVelocity().SizeSquared() < 100.f, TEXT("first pawn not moving"));
		AssertTrue(Pawns[1]->GetVelocity().SizeSquared() > 100.f, TEXT("second pawn moving"));
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}