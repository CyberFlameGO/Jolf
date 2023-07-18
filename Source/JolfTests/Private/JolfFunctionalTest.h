// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "FunctionalTest.h"
#include "JolfFunctionalTest.generated.h"

UCLASS()
class AJolfFunctionalTest : public AFunctionalTest
{
	GENERATED_BODY()

public: // Functions

	void SetNumLocalPlayers(int32 InNumPlayers);
	void RestartAllPlayers();
	TArray<class AJolfPlayerController*> GetPlayerControllers();
	TArray<class AJolfPlayerState*> GetPlayerStates();
	TArray<class AJolfPawn*> GetPawns();
};