// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSplitscreenMenu.h"
#include "JolfMenuStackRoot.h"
#include "Components/Button.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Components/CheckBox.h"
#include "JolfUserSettings.h"
#include "JolfChangeLevelButton.h"
#include "JolfLevelDefinition.h"
#include "JolfLocalPlayersList.h"
#include "JolfChangePlayerSettingsMenu.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfSplitscreenMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(LocalPlayersList))
	{
		LocalPlayersList->OnNumPlayersChanged.AddUObject(this, &UJolfSplitscreenMenu::OnNumPlayersChanged);
		LocalPlayersList->OnLocalPlayerClicked.AddUObject(this, &UJolfSplitscreenMenu::OnLocalPlayerClicked);
	}

	if (ensure(ChangeLevelButton))
		ChangeLevelButton->OnClicked.AddUObject(this, &UJolfSplitscreenMenu::OnChangeLevelClicked);

	const UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();
	if (ensure(OffsetGamepadUserIndexCheckBox))
	{
		OffsetGamepadUserIndexCheckBox->SetIsChecked(Settings->bOffsetGamepadUserIndex);
		OffsetGamepadUserIndexCheckBox->OnCheckStateChanged.AddDynamic(this, &UJolfSplitscreenMenu::OnOffsetGamepadUserIndexChecked);
	}

	if (ensure(TravelButton))
		TravelButton->OnClicked.AddDynamic(this, &UJolfSplitscreenMenu::OnTravelClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfSplitscreenMenu::OnBackClicked);
}

void UJolfSplitscreenMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();

	if (ensure(LocalPlayersList))
	{
		LocalPlayersList->SetNumPlayers(UserSettings->NumSplitscreenPlayers);
	}

	if (ensure(ChangeLevelButton))
	{
		UserSettings->OnPreferredLevelChanged.AddUObject(ChangeLevelButton, &UJolfChangeLevelButton::SetLevelDef);
		ChangeLevelButton->SetLevelDef(UserSettings->GetPreferredLevelDef());
	}
}

void UJolfSplitscreenMenu::NativeDestruct()
{
	UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();
	UserSettings->OnPreferredLevelChanged.RemoveAll(ChangeLevelButton);

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfSplitscreenMenu::OnNumPlayersChanged(int32 InValue)
{
	UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();
	UserSettings->NumSplitscreenPlayers = InValue;
	UserSettings->SaveSettings();
}

void UJolfSplitscreenMenu::OnLocalPlayerClicked(int32 InControllerId, UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	UJolfChangePlayerSettingsMenu* ChangeMenu = GetMenuRoot()->FindMenuByClass<UJolfChangePlayerSettingsMenu>();
	check(ChangeMenu);
	ChangeMenu->SetActivePlayerSettings(InPlayerSettings);
	ChangeMenu->OnSelectionChanged.BindUObject(this, &UJolfSplitscreenMenu::OnPlayerSettingsSelected, InControllerId);
	GetMenuRoot()->SetActiveMenu(TEXT("ChangePlayerSettings"));
}

void UJolfSplitscreenMenu::OnPlayerSettingsSelected(UJolfPerPlayerSettings* InPlayerSettings, int32 InControllerId)
{
	LocalPlayersList->SetPlayerSettings(InControllerId, InPlayerSettings);
}

void UJolfSplitscreenMenu::OnChangeLevelClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("LevelSelection"));
}

void UJolfSplitscreenMenu::OnOffsetGamepadUserIndexChecked(bool bInIsChecked)
{
	UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();
	Settings->bOffsetGamepadUserIndex = bInIsChecked;
	Settings->SaveSettings();
}

void UJolfSplitscreenMenu::OnTravelClicked()
{
	const UJolfLevelDefinition* LevelDef = ChangeLevelButton->GetLevelDef();
	FString LevelName = LevelDef ? LevelDef->Level.GetLongPackageName() : FString();
	
	int32 NumPlayers = LocalPlayersList->GetNumPlayers();
	for (int32 ControllerId = 1; ControllerId < NumPlayers; ++ControllerId)
	{
		FString Error;
		GetGameInstance()->CreateLocalPlayer(ControllerId, Error, /*bCreatePlayerController*/ false);
	}
	
	GEngine->SetClientTravel(GetWorld(), *FString::Printf(TEXT("%s?Collision=%i"), *LevelName, CollisionCheckBox->IsChecked()), TRAVEL_Absolute);
}

void UJolfSplitscreenMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfSplitscreenMenu::OnLevelSelectionChanged(UJolfLevelDefinition* InLevelDef)
{
	ChangeLevelButton->SetLevelDef(InLevelDef);
}