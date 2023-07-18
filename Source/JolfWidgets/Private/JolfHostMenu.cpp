// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfHostMenu.h"
#include "JolfMenuStackRoot.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "Components/CheckBox.h"
#include "JolfChangeLevelButton.h"
#include "JolfLevelDefinition.h"
#include "JolfUserSettings.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfHostMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(ChangeLevelButton))
		ChangeLevelButton->OnClicked.AddUObject(this, &UJolfHostMenu::OnChangeLevelClicked);

	if (ensure(TravelButton))
		TravelButton->OnClicked.AddDynamic(this, &UJolfHostMenu::OnTravelClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfHostMenu::OnBackClicked);
}

void UJolfHostMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensure(ChangeLevelButton))
	{
		UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();
		UserSettings->OnPreferredLevelChanged.AddUObject(ChangeLevelButton, &UJolfChangeLevelButton::SetLevelDef);
		ChangeLevelButton->SetLevelDef(UserSettings->GetPreferredLevelDef());
	}
}

void UJolfHostMenu::NativeDestruct()
{
	UJolfUserSettings* UserSettings = UJolfUserSettings::GetJolfUserSettings();
	UserSettings->OnPreferredLevelChanged.RemoveAll(ChangeLevelButton);

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfHostMenu::OnChangeLevelClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("LevelSelection"));
}

void UJolfHostMenu::OnTravelClicked()
{
	const UJolfLevelDefinition* LevelDef = ChangeLevelButton->GetLevelDef();
	FString LevelName = LevelDef ? LevelDef->Level.GetLongPackageName() : FString();
	GEngine->SetClientTravel(GetWorld(), *FString::Printf(TEXT("%s?listen?Collision=%i?UseActivePlayer=%i"), *LevelName, CollisionCheckBox->IsChecked(), UseActivePlayerCheckBox->IsChecked()), TRAVEL_Absolute);
}

void UJolfHostMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfHostMenu::OnLevelSelectionChanged(UJolfLevelDefinition* InLevelDef)
{
	ChangeLevelButton->SetLevelDef(InLevelDef);
}