// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSingleplayerMenu.h"
#include "JolfMenuStackRoot.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "Components/CheckBox.h"
#include "JolfChangeLevelButton.h"
#include "JolfLevelDefinition.h"
#include "JolfUserSettings.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfSingleplayerMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(ChangeLevelButton))
		ChangeLevelButton->OnClicked.AddUObject(this, &UJolfSingleplayerMenu::OnChangeLevelClicked);

	if (ensure(TravelButton))
		TravelButton->OnClicked.AddDynamic(this, &UJolfSingleplayerMenu::OnTravelClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfSingleplayerMenu::OnBackClicked);
}

void UJolfSingleplayerMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensure(ChangeLevelButton))
	{
		UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();
		UserSettings->OnPreferredLevelChanged.AddUObject(ChangeLevelButton, &UJolfChangeLevelButton::SetLevelDef);
		ChangeLevelButton->SetLevelDef(UserSettings->GetPreferredLevelDef());
	}
}

void UJolfSingleplayerMenu::NativeDestruct()
{
	UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();
	UserSettings->OnPreferredLevelChanged.RemoveAll(ChangeLevelButton);

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfSingleplayerMenu::OnChangeLevelClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("LevelSelection"));
}

void UJolfSingleplayerMenu::OnTravelClicked()
{
	const UJolfLevelDefinition* LevelDef = ChangeLevelButton->GetLevelDef();
	FString LevelName = LevelDef ? LevelDef->Level.GetLongPackageName() : FString();
	GEngine->SetClientTravel(GetWorld(), *FString::Printf(TEXT("%s"), *LevelName), TRAVEL_Absolute);
}

void UJolfSingleplayerMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfSingleplayerMenu::OnLevelSelectionChanged(UJolfLevelDefinition* InLevelDef)
{
	ChangeLevelButton->SetLevelDef(InLevelDef);
}