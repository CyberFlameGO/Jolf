// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPerPlayerSettings.h"

UJolfPerPlayerSettings::UJolfPerPlayerSettings() : Super()
{
	MousePitchSensitivity = 0.07f;
	MouseYawSensitivity = 0.07f;
	MousePowerSensitivity = 0.0025f;
	GamepadPitchDegreesPerSecond = 60.0f;
	GamepadYawDegreesPerSecond = 90.0f;
	GamepadPowerSensitivity = 1.0f;
	HorizontalFieldOfView = 90.f;
}

// Public Functions:

void UJolfPerPlayerSettings::SaveSettings()
{
	SaveConfig(CPF_Config, *GGameUserSettingsIni);
}

void UJolfPerPlayerSettings::SetPlayerName(const FString& InPlayerName)
{
	if (PlayerName == InPlayerName)
		return;

	PlayerName = InPlayerName;
	SaveSettings();

	OnPlayerNameChanged.Broadcast(this);
}

void UJolfPerPlayerSettings::AddUser(UJolfLocalPlayer* InLocalPlayer)
{
	check(InLocalPlayer);
	const int32 AddedIndex = LocalPlayerUsers.AddUnique(InLocalPlayer);
	check(AddedIndex != INDEX_NONE);
	OnLocalPlayerUsersChanged.Broadcast(this);
}

void UJolfPerPlayerSettings::RemoveUser(UJolfLocalPlayer* InLocalPlayer)
{
	check(InLocalPlayer);
	const int32 NumRemoved = LocalPlayerUsers.RemoveSingleSwap(InLocalPlayer);
	check(NumRemoved == 1);
	OnLocalPlayerUsersChanged.Broadcast(this);
}