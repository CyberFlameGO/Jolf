// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/GameUserSettings.h"
#include "JolfUserSettings.generated.h"

UCLASS()
class JOLF_API UJolfUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:

	UJolfUserSettings(const FObjectInitializer& ObjectInitializer);

public: // Functions

	static UJolfUserSettings* GetJolfUserSettings();

	class UJolfLevelDefinition* GetPreferredLevelDef();
	void SetPreferredLevelDef(class UJolfLevelDefinition* InLevelDef);

	class UJolfPerPlayerSettings* FindOrCreatePlayerSettings(int32 InControllerId);
	void CreateNewPlayerSettings();
	void SetPlayerSettingsAssociation(int32 InControllerId, class UJolfPerPlayerSettings* InPlayerSettings);
	void RemovePlayerSettings(class UJolfPerPlayerSettings* InPlayerSettings);

public: // Interfaces

	//~ Begin UGameUserSettings Interface
	void LoadSettings(bool bForceReload) override;
	void ValidateSettings() override;
	void ApplyNonResolutionSettings() override;
	void ResetToCurrentSettings() override;
	void SetToDefaults() override;
	//~ End UGameUserSettings Interface

public: // Properties

	/** Game replacement for UGameMapsSettings::bOffsetPlayerGamepadIds in order to support slate focus and hotseat. */
	UPROPERTY(Config)
	uint8 bOffsetGamepadUserIndex:1;

	UPROPERTY(Config)
	uint8 bEnableSceneProbe:1;

	UPROPERTY(Config)
	uint8 bUseFrameRateLimit:1;

	UPROPERTY(Config)
	int32 NumSplitscreenPlayers;

	UPROPERTY(Config)
	int32 NumHotseatPlayers;

	UPROPERTY(Config)
	float VolumeMultiplier;

	UPROPERTY(Config)
	float UnfocusedVolumeMultiplier;

	UPROPERTY(Config)
	float MusicVolumeMultiplier;

	UPROPERTY(Config)
	float LastConfirmedUserInterfaceScale;

	/** Driver for UUserInterfaceSettings::ApplicationScale. */
	UPROPERTY(Config)
	float UserInterfaceScale;

	UPROPERTY(Transient)
	TArray<class UJolfPerPlayerSettings*> PlayerSettingsInstances;

	DECLARE_MULTICAST_DELEGATE_OneParam(FPreferredLevelChanged, class UJolfLevelDefinition*);
	FPreferredLevelChanged OnPreferredLevelChanged;

	DECLARE_EVENT_OneParam(UJolfUserSettings, FPlayerSettingsEvent, class UJolfPerPlayerSettings*);
	FPlayerSettingsEvent OnPlayerSettingsAdded;
	FPlayerSettingsEvent OnPlayerSettingsRemoved;

protected: // Interfaces

	//~ Begin UGameUserSettings Interface
	float GetEffectiveFrameRateLimit() override;
	//~ End UGameUserSettings Interface

private: // Functions

	class UJolfPerPlayerSettings* FindPlayerSettingsInstanceByGuid(const FGuid& InGuid) const;
	/** Synchronize PlayerSettingsInstances with the array of PlayerSettingsGuids. */
	void SynchronizePlayerSettings();

	/** Workaround to apply sound mix during PIE. */
	void OnAudioDeviceCreated(Audio::FDeviceId InAudioDeviceId);

private: // Properties

	/** Most recently selected level definition. */
	UPROPERTY(Config)
	FGuid PreferredLevelGuid;

	/** Associates ControllerId to a per-player settings Guid. */
	UPROPERTY(Config)
	TMap<int32, FGuid> ControllerSettings;

	UPROPERTY(Config)
	TArray<FGuid> PlayerSettingsGuids;

	UPROPERTY(Transient)
	class USoundMix* SettingsSoundMix;

	UPROPERTY(Transient)
	class USoundClass* MasterSoundClass;
	
	UPROPERTY(Transient)
	class USoundClass* MusicSoundClass;
};