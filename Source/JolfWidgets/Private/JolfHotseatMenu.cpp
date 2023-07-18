// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfHotseatMenu.h"
#include "JolfMenuStackRoot.h"
#include "Components/Button.h"
#include "JolfViewportClient.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Components/CheckBox.h"
#include "JolfChangeLevelButton.h"
#include "JolfLevelDefinition.h"
#include "JolfUserSettings.h"
#include "JolfLocalPlayersList.h"
#include "JolfChangePlayerSettingsMenu.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfHotseatMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(LocalPlayersList))
	{
		LocalPlayersList->OnNumPlayersChanged.AddUObject(this, &UJolfHotseatMenu::OnNumPlayersChanged);
		LocalPlayersList->OnLocalPlayerClicked.AddUObject(this, &UJolfHotseatMenu::OnLocalPlayerClicked);
	}

	if (ensure(ChangeLevelButton))
		ChangeLevelButton->OnClicked.AddUObject(this, &UJolfHotseatMenu::OnChangeLevelClicked);

	if (ensure(TravelButton))
		TravelButton->OnClicked.AddDynamic(this, &UJolfHotseatMenu::OnTravelClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfHotseatMenu::OnBackClicked);
}

void UJolfHotseatMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();

	if (ensure(LocalPlayersList))
	{
		LocalPlayersList->SetNumPlayers(UserSettings->NumHotseatPlayers);
	}

	if (ensure(ChangeLevelButton))
	{
		UserSettings->OnPreferredLevelChanged.AddUObject(ChangeLevelButton, &UJolfChangeLevelButton::SetLevelDef);
		ChangeLevelButton->SetLevelDef(UserSettings->GetPreferredLevelDef());
	}
}

void UJolfHotseatMenu::NativeDestruct()
{
	UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();
	UserSettings->OnPreferredLevelChanged.RemoveAll(ChangeLevelButton);

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfHotseatMenu::OnNumPlayersChanged(int32 InValue)
{
	UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();
	UserSettings->NumHotseatPlayers = InValue;
	UserSettings->SaveSettings();
}

void UJolfHotseatMenu::OnLocalPlayerClicked(int32 InControllerId, UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	UJolfChangePlayerSettingsMenu* ChangeMenu = GetMenuRoot()->FindMenuByClass<UJolfChangePlayerSettingsMenu>();
	check(ChangeMenu);
	ChangeMenu->SetActivePlayerSettings(InPlayerSettings);
	ChangeMenu->OnSelectionChanged.BindUObject(this, &UJolfHotseatMenu::OnPlayerSettingsSelected, InControllerId);
	GetMenuRoot()->SetActiveMenu(TEXT("ChangePlayerSettings"));
}

void UJolfHotseatMenu::OnPlayerSettingsSelected(UJolfPerPlayerSettings* InPlayerSettings, int32 InControllerId)
{
	LocalPlayersList->SetPlayerSettings(InControllerId, InPlayerSettings);
}

void UJolfHotseatMenu::OnChangeLevelClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("LevelSelection"));
}

void UJolfHotseatMenu::OnTravelClicked()
{
	const UJolfLevelDefinition* LevelDef = ChangeLevelButton->GetLevelDef();
	FString LevelName = LevelDef ? LevelDef->Level.GetLongPackageName() : FString();
	CastChecked<UJolfViewportClient>(GetOwningLocalPlayer()->ViewportClient)->bHotseatMode = true;
	
	int32 NumPlayers = LocalPlayersList->GetNumPlayers();
	for (int32 ControllerId = 1; ControllerId < NumPlayers; ++ControllerId)
	{
		FString Error;
		GetGameInstance()->CreateLocalPlayer(ControllerId, Error, /*bCreatePlayerController*/ false);
	}

	GEngine->SetClientTravel(GetWorld(), *FString::Printf(TEXT("%s?Collision=%i?Hotseat=1"), *LevelName, CollisionCheckBox->IsChecked()), TRAVEL_Absolute);
}

void UJolfHotseatMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfHotseatMenu::OnLevelSelectionChanged(UJolfLevelDefinition* InLevelDef)
{
	ChangeLevelButton->SetLevelDef(InLevelDef);
}