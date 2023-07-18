// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPlayerState.h"
#include "Engine/World.h"
#include "JolfGameState.h"
#include "JolfHole.h"
#include "Net/UnrealNetwork.h"

// Public Interfaces:

//~ Begin UObject Interface
void AJolfPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AJolfPlayerState, StrokeIndex);
	DOREPLIFETIME(AJolfPlayerState, HoleIndex);
	DOREPLIFETIME(AJolfPlayerState, HoleScores);
}
//~ End UObject Interface

//~ Begin APlayerState Interface
void AJolfPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	OnPlayerNameChanged.Broadcast(this);
}
//~ End APlayerState Interface

// Public Functions:

AJolfHole* AJolfPlayerState::GetCurrentHoleActor() const
{
	if (AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
	{
		AJolfHole** CurrentHolePtr = GameState->Holes.FindByPredicate([this](AJolfHole* Hole)
		{
			return Hole->HoleIndex == HoleIndex;
		});
		return CurrentHolePtr ? *CurrentHolePtr : nullptr;
	}
	else
	{
		return nullptr;
	}
}

void AJolfPlayerState::SetHoleScore(int32 InHoleIndex, int32 InStrokes, float InTime)
{
	FJolfPlayerStateScore* HoleScore = HoleScores.FindByPredicate([InHoleIndex](FJolfPlayerStateScore& Entry)
	{
		return Entry.HoleIndex == InHoleIndex;
	});
	if (!HoleScore)
	{
		HoleScore = &HoleScores.AddZeroed_GetRef();
		HoleScore->HoleIndex = InHoleIndex;
	}
	check(HoleScore);
	HoleScore->Strokes = InStrokes;
	HoleScore->Time = InTime;
	OnRep_HoleScores();
}

// Private Functions:

void AJolfPlayerState::OnRep_HoleScores()
{
	OnHoleScoresChanged.Broadcast(this);
}