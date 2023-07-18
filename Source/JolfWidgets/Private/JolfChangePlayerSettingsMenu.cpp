// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfChangePlayerSettingsMenu.h"
#include "JolfMenuStackRoot.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "JolfUserSettings.h"
#include "JolfPerPlayerSettings.h"
#include "JolfEditableTextBox.h"
#include "JolfManagePlayerSettingsEntry.h"

// Public Functions:

void UJolfChangePlayerSettingsMenu::SetActivePlayerSettings(UJolfPerPlayerSettings* InPlayerSettings)
{
	if (ActivePlayerSettings == InPlayerSettings)
		return;

	ActivePlayerSettings = InPlayerSettings;

	if (ensure(NameTextBox))
		NameTextBox->SetText(FText::AsCultureInvariant(ActivePlayerSettings->GetPlayerName()));
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfChangePlayerSettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfChangePlayerSettingsMenu::OnBackClicked);

	if (ensure(CreateButton))
		CreateButton->OnClicked.AddDynamic(this, &UJolfChangePlayerSettingsMenu::OnCreateClicked);

	if (ensure(NameTextBox))
		NameTextBox->OnTextCommitted.AddDynamic(this, &UJolfChangePlayerSettingsMenu::OnTextCommitted);
}

void UJolfChangePlayerSettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UJolfUserSettings* SystemSettings = UJolfUserSettings::GetJolfUserSettings();
	OnPlayerSettingsAddedHandle = SystemSettings->OnPlayerSettingsAdded.AddUObject(this, &UJolfChangePlayerSettingsMenu::OnPlayerSettingsAdded);
	OnPlayerSettingsRemovedHandle = SystemSettings->OnPlayerSettingsRemoved.AddUObject(this, &UJolfChangePlayerSettingsMenu::OnPlayerSettingsRemoved);
	for (UJolfPerPlayerSettings* PlayerSettings : SystemSettings->PlayerSettingsInstances)
	{
		OnPlayerSettingsAdded(PlayerSettings);
	}
}

void UJolfChangePlayerSettingsMenu::NativeDestruct()
{
	UJolfUserSettings* SystemSettings = UJolfUserSettings::GetJolfUserSettings();
	SystemSettings->OnPlayerSettingsAdded.Remove(OnPlayerSettingsAddedHandle);
	SystemSettings->OnPlayerSettingsRemoved.Remove(OnPlayerSettingsRemovedHandle);

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfChangePlayerSettingsMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfChangePlayerSettingsMenu::OnCreateClicked()
{
	UJolfUserSettings* SystemSettings = UJolfUserSettings::GetJolfUserSettings();
	SystemSettings->CreateNewPlayerSettings();
}

void UJolfChangePlayerSettingsMenu::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (ActivePlayerSettings)
	{
		ActivePlayerSettings->SetPlayerName(Text.ToString());
	}
}

void UJolfChangePlayerSettingsMenu::OnSelectPlayerSettingsClicked(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	SetActivePlayerSettings(InPlayerSettings);
	OnSelectionChanged.ExecuteIfBound(InPlayerSettings);
}

void UJolfChangePlayerSettingsMenu::OnRemovePlayerSettingsClicked(class UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	UJolfUserSettings* SystemSettings = UJolfUserSettings::GetJolfUserSettings();
	SystemSettings->RemovePlayerSettings(InPlayerSettings);
}

void UJolfChangePlayerSettingsMenu::OnPlayerSettingsAdded(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	UJolfManagePlayerSettingsEntry* EntryInstance = CreateWidget<UJolfManagePlayerSettingsEntry>(this, EntryClass);
	if (ensure(EntryInstance))
	{
		EntryInstance->SetPlayerSettings(InPlayerSettings);
		EntryInstance->OnSelectClicked.AddUObject(this, &UJolfChangePlayerSettingsMenu::OnSelectPlayerSettingsClicked);
		EntryInstance->OnRemoveClicked.AddUObject(this, &UJolfChangePlayerSettingsMenu::OnRemovePlayerSettingsClicked);
		OptionsScrollBox->AddChild(EntryInstance);
		Entries.Add(EntryInstance);
	}
}

void UJolfChangePlayerSettingsMenu::OnPlayerSettingsRemoved(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	for (int32 Index = Entries.Num() - 1; Index >= 0; --Index)
	{
		UJolfManagePlayerSettingsEntry* Entry = Entries[Index];
		check(Entry);
		if (Entry->GetPlayerSettings() == InPlayerSettings)
		{
			Entries.RemoveAt(Index);
			bool bRemovedWidget = OptionsScrollBox->RemoveChildAt(Index);
			check(bRemovedWidget);
			return;
		}
	}
}