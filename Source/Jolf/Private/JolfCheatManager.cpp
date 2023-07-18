// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfCheatManager.h"
#include "JolfPlayerController.h"
#include "JolfPlayerState.h"

// Private Functions:

void UJolfCheatManager::JolfGoToHole(int32 HoleIndex)
{
#if !UE_BUILD_SHIPPING
	AJolfPlayerController* PC = CastChecked<AJolfPlayerController>(GetOuter());
	AJolfPlayerState* PS = PC->GetPlayerState<AJolfPlayerState>();
	PS->HoleIndex = HoleIndex;
	PC->ServerRestartHole();
#endif // !UE_BUILD_SHIPPING
}

void UJolfCheatManager::ChangePlayerName(const FString& PlayerName)
{
#if !UE_BUILD_SHIPPING
	AJolfPlayerController* PC = CastChecked<AJolfPlayerController>(GetOuter());
	PC->ServerChangeName(PlayerName);
#endif // !UE_BUILD_SHIPPING
}