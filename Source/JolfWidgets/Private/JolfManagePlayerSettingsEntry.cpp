// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfManagePlayerSettingsEntry.h"
#include "JolfPerPlayerSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

// Public Functions:

void UJolfManagePlayerSettingsEntry::SetPlayerSettings(UJolfPerPlayerSettings* InPlayerSettings)
{
	if (PlayerSettings == InPlayerSettings)
		return;

	if (PlayerSettings)
	{
		if (OnPlayerNameChangedHandle.IsValid())
		{
			PlayerSettings->OnPlayerNameChanged.Remove(OnPlayerNameChangedHandle);
			OnPlayerNameChangedHandle.Reset();
		}

		if (OnNumUsersChangedHandle.IsValid())
		{
			PlayerSettings->OnLocalPlayerUsersChanged.Remove(OnNumUsersChangedHandle);
			OnNumUsersChangedHandle.Reset();
		}
	}

	PlayerSettings = InPlayerSettings;

	if (PlayerSettings)
	{
		OnPlayerNameChangedHandle = PlayerSettings->OnPlayerNameChanged.AddUObject(this, &UJolfManagePlayerSettingsEntry::OnPlayerNameChanged);
		OnPlayerNameChanged(PlayerSettings);
		OnNumUsersChangedHandle = PlayerSettings->OnLocalPlayerUsersChanged.AddUObject(this, &UJolfManagePlayerSettingsEntry::OnNumUsersChanged);
		OnNumUsersChanged(InPlayerSettings);
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfManagePlayerSettingsEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(SelectButton))
		SelectButton->OnClicked.AddDynamic(this, &UJolfManagePlayerSettingsEntry::OnSelectButtonClicked);

	if (ensure(RemoveButton))
		RemoveButton->OnClicked.AddDynamic(this, &UJolfManagePlayerSettingsEntry::OnRemoveButtonClicked);
}

void UJolfManagePlayerSettingsEntry::NativeDestruct()
{
	SetPlayerSettings(nullptr);
	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfManagePlayerSettingsEntry::OnSelectButtonClicked()
{
	OnSelectClicked.Broadcast(PlayerSettings);
}

void UJolfManagePlayerSettingsEntry::OnRemoveButtonClicked()
{
	OnRemoveClicked.Broadcast(PlayerSettings);
}

void UJolfManagePlayerSettingsEntry::OnPlayerNameChanged(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(PlayerSettings == InPlayerSettings);

	if (ensure(PlayerNameTextBlock))
		PlayerNameTextBlock->SetText(FText::AsCultureInvariant(InPlayerSettings->GetPlayerName()));
}

void UJolfManagePlayerSettingsEntry::OnNumUsersChanged(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(PlayerSettings == InPlayerSettings);

	if (ensure(RemoveButton))
	{
		const int32 NumUsers = InPlayerSettings->GetNumLocalPlayerUsers();
		if (NumUsers > 0)
		{
			RemoveButton->SetToolTipText(FText::FormatOrdered(NSLOCTEXT("JolfPlayerSettings", "CannotRemove", "Used by {0} {0}|plural(one=player,other=players)"), FText::AsNumber(NumUsers)));
			RemoveButton->SetIsEnabled(false);
		}
		else
		{
			RemoveButton->SetToolTipText(FText::GetEmpty());
			RemoveButton->SetIsEnabled(true);
		}
	}
}