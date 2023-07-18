// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfMainMenu.h"
#include "Engine/Engine.h"
#include "Components/Button.h"
#include "Engine/LocalPlayer.h"
#include "JolfViewportClient.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "HAL/PlatformMisc.h"
#include "JolfMenuStackRoot.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(SingleplayerButton))
		SingleplayerButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnSingleplayerClicked);

	if (ensure(SplitscreenButton))
		SplitscreenButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnSplitscreenClicked);

	if (ensure(HotseatButton))
		HotseatButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnHotseatClicked);

	if (ensure(HostButton))
		HostButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnHostClicked);

	if (ensure(JoinButton))
		JoinButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnJoinClicked);

	if (ensure(SystemSettingsButton))
		SystemSettingsButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnSystemSettingsClicked);

	if (ensure(OptionsButton))
		OptionsButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnOptionsClicked);

	if (ensure(QuitButton))
		QuitButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnQuitClicked);

	if (ensure(CreditsButton))
		CreditsButton->OnClicked.AddDynamic(this, &UJolfMainMenu::OnCreditsClicked);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfMainMenu::OnSingleplayerClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("Singleplayer"));
}

void UJolfMainMenu::OnSplitscreenClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("Splitscreen"));
}

void UJolfMainMenu::OnHotseatClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("Hotseat"));
}

void UJolfMainMenu::OnHostClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("Host"));
}

void UJolfMainMenu::OnJoinClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("Join"));
}

void UJolfMainMenu::OnSystemSettingsClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("SystemSettings"));
}

void UJolfMainMenu::OnOptionsClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("Options"));
}

void UJolfMainMenu::OnQuitClicked()
{
	FPlatformMisc::RequestExit(false);
}

void UJolfMainMenu::OnCreditsClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("Credits"));
}
