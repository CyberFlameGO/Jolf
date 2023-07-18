// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfFunctionalTest.h"
#include "Engine/GameInstance.h"
#include "JolfPlayerController.h"
#include "JolfPlayerState.h"
#include "JolfGameMode.h"
#include "JolfPawn.h"
#include "JolfViewportClient.h"

// Public Functions:

void AJolfFunctionalTest::SetNumLocalPlayers(int32 InNumPlayers)
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	int32 OldNumPlayers = GameInstance->GetNumLocalPlayers();

	if (InNumPlayers > OldNumPlayers)
	{
		// Add players.
		for (int32 ControllerIndex = OldNumPlayers; ControllerIndex < InNumPlayers; ++ControllerIndex)
		{
			FString Error;
			GameInstance->CreateLocalPlayer(ControllerIndex, Error, true);
			AssertEqual_String(Error, FString(), FString::Printf(TEXT("add controller %i error"), ControllerIndex));
		}
	}
	else if (InNumPlayers < OldNumPlayers)
	{
		// Remove players.
		for (int32 ControllerIndex = OldNumPlayers - 1; ControllerIndex >= InNumPlayers; --ControllerIndex)
		{
			class ULocalPlayer* ExistingPlayer = GameInstance->GetLocalPlayerByIndex(ControllerIndex);
			if (AssertIsValid(ExistingPlayer, FString::Printf(TEXT("remove player %i exists"), ControllerIndex)))
			{
				bool bSuccess = GameInstance->RemoveLocalPlayer(ExistingPlayer);
				AssertTrue(bSuccess, FString::Printf(TEXT("removed controller %i"), ControllerIndex));
			}
		}
	}

	AssertEqual_Int(GameInstance->GetNumLocalPlayers(), InNumPlayers, TEXT("updated num local players"));

	if (UJolfViewportClient* ViewportClient = Cast<UJolfViewportClient>(GetWorld()->GetGameViewport()))
	{
		ViewportClient->bHotseatMode = false;
		ViewportClient->SetHotseatIndex(INDEX_NONE);
	}
}

void AJolfFunctionalTest::RestartAllPlayers()
{
	AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>();

	TArray<AJolfPlayerController*> PlayerControllers = GetPlayerControllers();
	for (AJolfPlayerController* PC : PlayerControllers)
	{
		AJolfPlayerState* PS = PC->GetPlayerState<AJolfPlayerState>();
		PS->HoleIndex = 0;
		PC->StartSpot.Reset();

		if (APawn* Pawn = PC->GetPawn())
		{
			Pawn->Destroy();
		}

		// Restart after destroying pawn, otherwise existing pawn is reused.
		GameMode->RestartPlayer(PC);
	}
}

TArray<AJolfPlayerController*> AJolfFunctionalTest::GetPlayerControllers()
{
	TArray<AJolfPlayerController*> PlayerControllerList;

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		if (AJolfPlayerController* PC = Cast<AJolfPlayerController>(Iter->Get()))
		{
			PlayerControllerList.Add(PC);
		}
	}

	return PlayerControllerList;
}

TArray<AJolfPlayerState*> AJolfFunctionalTest::GetPlayerStates()
{
	TArray<AJolfPlayerState*> PlayerStateList;

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		if (AJolfPlayerState* PS = (*Iter)->GetPlayerState<AJolfPlayerState>())
		{
			PlayerStateList.Add(PS);
		}
	}

	return PlayerStateList;
}

TArray<AJolfPawn*> AJolfFunctionalTest::GetPawns()
{
	TArray<AJolfPawn*> PawnsList;

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		if (AJolfPawn* Pawn = (*Iter)->GetPawn<AJolfPawn>())
		{
			PawnsList.Add(Pawn);
		}
	}

	return PawnsList;
}