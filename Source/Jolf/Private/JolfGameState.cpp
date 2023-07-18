// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfGameState.h"
#include "JolfHole.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "JolfPlayerState.h"
#include "JolfViewportClient.h"
#include "Engine/World.h"
#include "JolfPawn.h"
#include "JolfPlayerController.h"

AJolfGameState::AJolfGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bEnablePawnCollision(true)
	, bShouldWaitForAllPlayersToFinishHole(true)
	, NumFreeMulligans(1)
{}

// Public Functions:

void AJolfGameState::SetSequentialTurns(bool bInSequentialTurns)
{
	bSequentialTurns = bInSequentialTurns;
	OnRep_SequentialTurns();
}

void AJolfGameState::SetEnablePawnCollision(bool bInEnablePawnCollision)
{
	bEnablePawnCollision = bInEnablePawnCollision;
	OnRep_EnablePawnCollision();
}

void AJolfGameState::SetWaitForAllPlayers(bool bInWaitForAllPlayers)
{
	bShouldWaitForAllPlayersToFinishHole = bInWaitForAllPlayers;
	OnRep_WaitForAllPlayers();
}

void AJolfGameState::SetAllowStrokeWhileMoving(bool bInAllowStrokeWhileMoving)
{
	bAllowStrokeWhileMoving = bInAllowStrokeWhileMoving;
	OnRep_AllowStrokeWhileMoving();
}

void AJolfGameState::SetActivePlayerState(AJolfPlayerState* InPlayerState)
{
	ActivePlayerState = InPlayerState;

	if (ActivePlayerState)
	{
		if (AJolfPawn* Pawn = ActivePlayerState->GetPawn<AJolfPawn>())
		{
			// Reset allowed to move.
			Pawn->bUsedSequentialTurnMove = false;
		}
	}

	OnRep_ActivePlayerState();
}

// Public Interfaces:

//~ Begin UObject Interface
void AJolfGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AJolfGameState, bEnablePawnCollision);
	DOREPLIFETIME(AJolfGameState, bSequentialTurns);
	DOREPLIFETIME(AJolfGameState, bShouldWaitForAllPlayersToFinishHole);
	DOREPLIFETIME(AJolfGameState, bAllowStrokeWhileMoving);
	DOREPLIFETIME(AJolfGameState, NumFreeMulligans);
	DOREPLIFETIME(AJolfGameState, ActivePlayerState);
}
//~ End UObject Interface

//~ Begin AActor Interface
void AJolfGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (TActorIterator<AJolfHole> Iter(GetWorld()); Iter; ++Iter)
	{
		Holes.AddUnique(*Iter);
	}
}
//~ End AActor Interface

//~ Begin AGameStateBase Interface
void AJolfGameState::AddPlayerState(APlayerState* PlayerState)
{
	int32 OldNum = PlayerArray.Num();
	Super::AddPlayerState(PlayerState);
	if (PlayerArray.Num() > OldNum)
	{
		if (AJolfPlayerState* TypedPlayerState = Cast<AJolfPlayerState>(PlayerState))
		{
			OnPlayerAdded.Broadcast(TypedPlayerState);
		}
	}
}

void AJolfGameState::RemovePlayerState(APlayerState* PlayerState)
{
	int32 OldNum = PlayerArray.Num();
	Super::RemovePlayerState(PlayerState);
	if (PlayerArray.Num() < OldNum)
	{
		if (AJolfPlayerState* TypedPlayerState = Cast<AJolfPlayerState>(PlayerState))
		{
			OnPlayerRemoved.Broadcast(TypedPlayerState);
		}
	}
}
//~ End AGameStateBase Interface

// Private Functions:

void AJolfGameState::OnRep_EnablePawnCollision()
{
	OnEnablePawnCollisionChanged.Broadcast();
}

void AJolfGameState::OnRep_SequentialTurns()
{
	OnSequentialTurnsChanged.Broadcast();

	if (bSequentialTurns)
	{
		// All players should watch the active player, if available.

		if (ActivePlayerState)
		{
			for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
			{
				(*Iter)->AutoManageActiveCameraTarget(ActivePlayerState);
			}
		}
	}
	else
	{
		// Players can return to viewing their own pawn.

		for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
		{
			(*Iter)->AutoManageActiveCameraTarget((*Iter)->PlayerState);
		}
	}
}

void AJolfGameState::OnRep_WaitForAllPlayers()
{
	OnWaitForAllPlayersChanged.Broadcast();
}

void AJolfGameState::OnRep_NumFreeMulligans()
{
	OnNumFreeMulligansChanged.Broadcast();
}

void AJolfGameState::OnRep_ActivePlayerState()
{
	if (IsNetMode(ENetMode::NM_DedicatedServer))
		return;

	if (ActivePlayerState == nullptr)
		return;

	if (UJolfViewportClient* ViewportClient = Cast<UJolfViewportClient>(GetWorld()->GetGameViewport()))
	{
		// In hotseat mode change the focused player.
		if (ViewportClient->IsHotseatMode())
		{
			AJolfPlayerController* OldHotseatPC = ViewportClient->GetHotseatPlayerController();

			ViewportClient->SetHotseatIndex(PlayerArray.IndexOfByKey(ActivePlayerState));

			AJolfPlayerController* NewHotseatPC = ViewportClient->GetHotseatPlayerController();
			if (OldHotseatPC != NewHotseatPC)
			{
				NewHotseatPC->BecomeHotseatActivePlayer();
			}
		}
		else if (bSequentialTurns)
		{
			for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
			{
				(*Iter)->AutoManageActiveCameraTarget(ActivePlayerState);
			}
		}
	}
}

void AJolfGameState::OnRep_AllowStrokeWhileMoving()
{
	OnAllowStrokeWhileMovingChanged.Broadcast();
}