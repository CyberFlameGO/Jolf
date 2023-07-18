// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/CheatManager.h"
#include "JolfCheatManager.generated.h"

UCLASS()
class UJolfCheatManager : public UCheatManager
{
	GENERATED_BODY()

private: // Functions

	UFUNCTION(Exec)
	void JolfGoToHole(int32 HoleIndex);
	
	UFUNCTION(Exec)
	void ChangePlayerName(const FString& PlayerName);
};