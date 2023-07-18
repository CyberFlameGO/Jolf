// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfLocalPlayerEntry.h"
#include "JolfPerPlayerSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

// Public Functions:

void UJolfLocalPlayerEntry::SetPlayerSettings(UJolfPerPlayerSettings* InPlayerSettings)
{
	if (PlayerSettings == InPlayerSettings)
		return;

	if (PlayerSettings && OnPlayerNameChangedHandle.IsValid())
	{
		PlayerSettings->OnPlayerNameChanged.Remove(OnPlayerNameChangedHandle);
		OnPlayerNameChangedHandle.Reset();
	}

	PlayerSettings = InPlayerSettings;

	if (PlayerSettings)
	{
		OnPlayerNameChangedHandle = PlayerSettings->OnPlayerNameChanged.AddUObject(this, &UJolfLocalPlayerEntry::OnPlayerNameChanged);
		OnPlayerNameChanged(PlayerSettings);
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfLocalPlayerEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(Button))
		Button->OnClicked.AddDynamic(this, &UJolfLocalPlayerEntry::OnClicked);
}

void UJolfLocalPlayerEntry::NativeDestruct()
{
	if (PlayerSettings && OnPlayerNameChangedHandle.IsValid())
	{
		PlayerSettings->OnPlayerNameChanged.Remove(OnPlayerNameChangedHandle);
		OnPlayerNameChangedHandle.Reset();
	}

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfLocalPlayerEntry::OnClicked()
{
	OnPlayerSettingsClicked.Broadcast(PlayerSettings);
}

void UJolfLocalPlayerEntry::OnPlayerNameChanged(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(PlayerSettings == InPlayerSettings);

	if (ensure(PlayerNameTextBlock))
		PlayerNameTextBlock->SetText(FText::AsCultureInvariant(InPlayerSettings->GetPlayerName()));
}