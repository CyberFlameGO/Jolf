// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfUserSettings.h"
#include "Engine/Engine.h"
#include "HAL/IConsoleManager.h"
#include "JolfLevelDefinition.h"
#include "JolfPerPlayerSettings.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "AudioDeviceManager.h"
#include "AudioDevice.h"
#include "Engine/UserInterfaceSettings.h"

UJolfUserSettings::UJolfUserSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		SettingsSoundMix = ObjectInitializer.CreateDefaultSubobject<USoundMix>(this, TEXT("SettingsMix"), /*bTransient*/ true);

		FSoundClassAdjuster& RootSoundClassAdjuster = SettingsSoundMix->SoundClassEffects.AddDefaulted_GetRef();
		RootSoundClassAdjuster.SoundClassObject = LoadObject<USoundClass>(nullptr, TEXT("/Game/SoundClasses/Root"));
		RootSoundClassAdjuster.VolumeAdjuster = 1.f;
		RootSoundClassAdjuster.PitchAdjuster = 1.f;
		RootSoundClassAdjuster.bApplyToChildren = true;

		FSoundClassAdjuster& MusicSoundClassAdjuster = SettingsSoundMix->SoundClassEffects.AddDefaulted_GetRef();
		MusicSoundClassAdjuster.SoundClassObject = LoadObject<USoundClass>(nullptr, TEXT("/Game/SoundClasses/Music"));
		MusicSoundClassAdjuster.VolumeAdjuster = 1.f;
		MusicSoundClassAdjuster.PitchAdjuster = 1.f;

		FAudioDeviceManagerDelegates::OnAudioDeviceCreated.AddUObject(this, &UJolfUserSettings::OnAudioDeviceCreated);
	}
}

// Public Functions:

UJolfUserSettings* UJolfUserSettings::GetJolfUserSettings()
{
	return CastChecked<UJolfUserSettings>(GEngine->GetGameUserSettings());
}

UJolfLevelDefinition* UJolfUserSettings::GetPreferredLevelDef()
{
	return UJolfLevelDefinition::FindByGuid(PreferredLevelGuid);
}

void UJolfUserSettings::SetPreferredLevelDef(UJolfLevelDefinition* InLevelDef)
{
	OnPreferredLevelChanged.Broadcast(InLevelDef);
	PreferredLevelGuid = InLevelDef ? InLevelDef->LevelGuid : FGuid();
	SaveSettings();
}

UJolfPerPlayerSettings* UJolfUserSettings::FindOrCreatePlayerSettings(int32 InControllerId)
{
	FGuid& PlayerSettingsGuid = ControllerSettings.FindOrAdd(InControllerId);
	if (!PlayerSettingsGuid.IsValid())
	{
		PlayerSettingsGuid = FGuid::NewGuid();
		PlayerSettingsGuids.Add(PlayerSettingsGuid);
		SaveSettings();
	}
	else if (PlayerSettingsGuids.AddUnique(PlayerSettingsGuid) != INDEX_NONE)
	{
		SaveSettings();
	}

	if (UJolfPerPlayerSettings* ExistingPlayerSettings = FindPlayerSettingsInstanceByGuid(PlayerSettingsGuid))
	{
		return ExistingPlayerSettings;
	}
	else
	{
		const FString SettingsName = FString::Printf(TEXT("LocalPlayer_%s"), *PlayerSettingsGuid.ToString());
		UJolfPerPlayerSettings* NewPlayerSettings = NewObject<UJolfPerPlayerSettings>(this, *SettingsName);
		NewPlayerSettings->LoadConfig(NewPlayerSettings->GetClass(), *GGameUserSettingsIni);
		NewPlayerSettings->Guid = PlayerSettingsGuid;
		if (NewPlayerSettings->GetPlayerName().IsEmpty())
		{
			NewPlayerSettings->SetPlayerName(FString::Printf(TEXT("Player %i"), (InControllerId + 1)));
		}
		PlayerSettingsInstances.Add(NewPlayerSettings);
		OnPlayerSettingsAdded.Broadcast(NewPlayerSettings);
		return NewPlayerSettings;
	}
}

void UJolfUserSettings::CreateNewPlayerSettings()
{
	FGuid NewGuid = FGuid::NewGuid();
	PlayerSettingsGuids.Add(NewGuid);
	SaveSettings();

	const FString SettingsName = FString::Printf(TEXT("LocalPlayer_%s"), *NewGuid.ToString());
	UJolfPerPlayerSettings* NewPlayerSettings = NewObject<UJolfPerPlayerSettings>(this, *SettingsName);
	NewPlayerSettings->Guid = NewGuid;
	NewPlayerSettings->SetPlayerName(TEXT("New Profile"));
	PlayerSettingsInstances.Add(NewPlayerSettings);
	OnPlayerSettingsAdded.Broadcast(NewPlayerSettings);
}

void UJolfUserSettings::SetPlayerSettingsAssociation(int32 InControllerId, UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	check(InPlayerSettings->Guid.IsValid());
	ControllerSettings.FindOrAdd(InControllerId) = InPlayerSettings->Guid;
	SaveSettings();
}

void UJolfUserSettings::RemovePlayerSettings(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	check(!InPlayerSettings->IsPendingKill());
	check(InPlayerSettings->GetNumLocalPlayerUsers() == 0);
	const int32 NumGuidsRemoved = PlayerSettingsGuids.RemoveSingle(InPlayerSettings->Guid);
	check(NumGuidsRemoved == 1);
	const int32 NumInstancesRemoved = PlayerSettingsInstances.RemoveSingle(InPlayerSettings);
	check(NumInstancesRemoved == 1);
	checkf(PlayerSettingsInstances.Num() == PlayerSettingsGuids.Num(), TEXT("PlayerSettingsInstances: %i PlayerSettingsGuids: %i"), PlayerSettingsInstances.Num(), PlayerSettingsGuids.Num());
	OnPlayerSettingsRemoved.Broadcast(InPlayerSettings);
	InPlayerSettings->MarkPendingKill();
}

// Public Interfaces:

//~ Begin UGameUserSettings Interface
void UJolfUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	SynchronizePlayerSettings();
}

void UJolfUserSettings::ValidateSettings()
{
	Super::ValidateSettings();
	UserInterfaceScale = FMath::Clamp(UserInterfaceScale, 0.5f, 2.f);
}

void UJolfUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	
	if (!IsRunningDedicatedServer())
	{
		static IConsoleVariable* SceneProbeCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("j.SceneProbe.Enable"));
		if (ensure(SceneProbeCVar))
		{
			SceneProbeCVar->Set(bEnableSceneProbe);
		}
	
		// Note - FApp::SetVolumeMultiplier is used by platforms to implement focused/unfocused volume, not intended for game use.
		FApp::SetUnfocusedVolumeMultiplier(UnfocusedVolumeMultiplier);

		if (SettingsSoundMix)
		{
			SettingsSoundMix->SoundClassEffects[0].VolumeAdjuster = VolumeMultiplier;
			SettingsSoundMix->SoundClassEffects[1].VolumeAdjuster = MusicVolumeMultiplier;

			if (FAudioDeviceManager* AudioDeviceManager = GEngine->GetAudioDeviceManager())
			{
				for (FAudioDevice* AudioDevice : AudioDeviceManager->GetAudioDevices())
				{
					if (AudioDevice == nullptr)
						continue; // Was running into nullptr crash, so just to be safe...

					AudioDevice->PushSoundMixModifier(SettingsSoundMix);
				}
			}
		}

		GetMutableDefault<UUserInterfaceSettings>()->ApplicationScale = UserInterfaceScale;
	}
}

void UJolfUserSettings::ResetToCurrentSettings()
{
	Super::ResetToCurrentSettings();
	
	if (!IsRunningDedicatedServer())
	{
		static IConsoleVariable* SceneProbeCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("j.SceneProbe.Enable"));
		if (ensure(SceneProbeCVar))
		{
			bEnableSceneProbe = SceneProbeCVar->GetBool();
		}

		UnfocusedVolumeMultiplier = FApp::GetUnfocusedVolumeMultiplier();

		LastConfirmedUserInterfaceScale = GetDefault<UUserInterfaceSettings>()->ApplicationScale;
		UserInterfaceScale = LastConfirmedUserInterfaceScale;
	}
}

void UJolfUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	// Note SetToDefaults is called during base constructor, but that does not call subclass implementation.
	// Subclass implementation is then called by engine after construction.

	bEnableSceneProbe = true;
	bUseFrameRateLimit = false;

	NumSplitscreenPlayers = 2;
	NumHotseatPlayers = 2;

	VolumeMultiplier = 1.f;
	UnfocusedVolumeMultiplier = 0.f;
	MusicVolumeMultiplier = 0.25f;

	LastConfirmedUserInterfaceScale = 1.0f;
	UserInterfaceScale = LastConfirmedUserInterfaceScale;

	FString DefaultLevelGuid;
	if (GConfig->GetString(TEXT("Jolf"), TEXT("DefaultLevelGuid"), DefaultLevelGuid, GGameIni))
	{
		if (!FGuid::Parse(DefaultLevelGuid, PreferredLevelGuid))
		{
			UE_LOG(LogTemp, Warning, TEXT("Unable to parse Jolf DefaultLevelGuid \"%s\""), *DefaultLevelGuid);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Jolf DefaultLevelGuid not specified"));
	}
}
//~ End UGameUserSettings Interface

// Protected Interfaces:

//~ Begin UGameUserSettings Interface
float UJolfUserSettings::GetEffectiveFrameRateLimit()
{
	return bUseFrameRateLimit ? FMath::Max(FrameRateLimit, 15.f) : 0.f;
}
//~ End UGameUserSettings Interface

// Private Functions:

UJolfPerPlayerSettings* UJolfUserSettings::FindPlayerSettingsInstanceByGuid(const FGuid& InGuid) const
{
	for (UJolfPerPlayerSettings* Instance : PlayerSettingsInstances)
	{
		if (ensure(Instance) && Instance->Guid == InGuid)
		{
			return Instance;
		}
	}

	return nullptr;
}

void UJolfUserSettings::SynchronizePlayerSettings()
{
	PlayerSettingsInstances.Reserve(PlayerSettingsGuids.Num());

	for (const FGuid& Guid : PlayerSettingsGuids)
	{
		UJolfPerPlayerSettings* ExistingSettings = FindPlayerSettingsInstanceByGuid(Guid);
		if (ExistingSettings == nullptr)
		{
			const FString SettingsName = FString::Printf(TEXT("LocalPlayer_%s"), *Guid.ToString());
			UJolfPerPlayerSettings* PlayerSettings = NewObject<UJolfPerPlayerSettings>(this, *SettingsName);
			PlayerSettings->LoadConfig(PlayerSettings->GetClass(), *GGameUserSettingsIni);
			PlayerSettings->Guid = Guid;
			PlayerSettingsInstances.Add(PlayerSettings);
			OnPlayerSettingsAdded.Broadcast(PlayerSettings);
		}
	}

	checkf(PlayerSettingsInstances.Num() == PlayerSettingsGuids.Num(), TEXT("PlayerSettingsInstances: %i PlayerSettingsGuids: %i"), PlayerSettingsInstances.Num(), PlayerSettingsGuids.Num());
}

void UJolfUserSettings::OnAudioDeviceCreated(Audio::FDeviceId InAudioDeviceId)
{
	// UE_LOG(LogTemp, Display, TEXT("Jolf OnAudioDeviceCreated %i"), InAudioDeviceId);
	if (!IsRunningDedicatedServer() && SettingsSoundMix)
	{
		if (FAudioDeviceManager* AudioDeviceManager = GEngine->GetAudioDeviceManager())
		{
			FAudioDevice* AudioDevice = AudioDeviceManager->GetAudioDeviceRaw(InAudioDeviceId);
			if (ensure(AudioDevice))
			{
				AudioDevice->PushSoundMixModifier(SettingsSoundMix);
			}
		}
	}
}