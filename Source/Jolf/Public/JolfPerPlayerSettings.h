// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "UObject/Object.h"
#include "JolfPerPlayerSettings.generated.h"

UCLASS(Config = GameUserSettings, PerObjectConfig, ConfigDoNotCheckDefaults)
class JOLF_API UJolfPerPlayerSettings : public UObject
{
	GENERATED_BODY()

public:

	friend class UJolfLocalPlayer;
	UJolfPerPlayerSettings();

public: // Functions

	void SaveSettings();

	const FString& GetPlayerName() const { return PlayerName; }
	void SetPlayerName(const FString& InPlayerName);

	int32 GetNumLocalPlayerUsers() const { return LocalPlayerUsers.Num(); }

public: // Properties

	DECLARE_EVENT_OneParam(UJolfPerPlayerSettings, FOnPlayerNameChangedEvent, class UJolfPerPlayerSettings*);
	FOnPlayerNameChangedEvent OnPlayerNameChanged;
	FOnPlayerNameChangedEvent OnLocalPlayerUsersChanged;

	/** Unique identifier. Allows player settings to be renamed. */
	UPROPERTY()
	FGuid Guid;

	/** Degrees multiplied by mouse Y axis value. */
	UPROPERTY(Config)
	float MousePitchSensitivity;

	/** Degrees multiplied by mouse X axis value. */
	UPROPERTY(Config)
	float MouseYawSensitivity;
	
	/** Multiplied by mouse Y axis value. */
	UPROPERTY(Config)
	float MousePowerSensitivity;
	
	/** Multiplied by gamepad Y axis value by delta time. */
	UPROPERTY(Config)
	float GamepadPitchDegreesPerSecond;

	/** Multiplied by gamepad X axis value by delta time. */
	UPROPERTY(Config)
	float GamepadYawDegreesPerSecond;
	
	/** Multiplied by gamepad Y axis value by delta time. */
	UPROPERTY(Config)
	float GamepadPowerSensitivity;
	
	UPROPERTY(Config)
	float HorizontalFieldOfView;
	
	/**
	 * Does this player want the % shown under the power bar in the HUD?
	 * Off by default because it makes repeatable shots much easier.
	 */
	UPROPERTY(Config)
	uint8 bShowStrokePowerPercentage:1;

private: // Functions:

	void AddUser(class UJolfLocalPlayer* InLocalPlayer);
	void RemoveUser(class UJolfLocalPlayer* InLocalPlayer);

private: // Properties

	/** Customizable display name. */
	UPROPERTY(Config)
	FString PlayerName;

	/** Local players currently referencing this. */
	UPROPERTY(Transient)
	TArray<class UJolfLocalPlayer*> LocalPlayerUsers;
};