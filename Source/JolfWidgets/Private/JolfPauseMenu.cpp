// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPauseMenu.h"
#include "Components/Button.h"
#include "HAL/PlatformMisc.h"
#include "JolfPlayerController.h"
#include "JolfViewportClient.h"
#include "JolfMenuStackRoot.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(ResumeButton))
		ResumeButton->OnClicked.AddDynamic(this, &UJolfPauseMenu::OnResumeClicked);

	if (ensure(MainMenuButton))
		MainMenuButton->OnClicked.AddDynamic(this, &UJolfPauseMenu::OnMainMenuClicked);

	if (ensure(RestartHoleButton))
		RestartHoleButton->OnClicked.AddDynamic(this, &UJolfPauseMenu::OnRestartHoleClicked);

	if (ensure(ChangeLevelButton))
		ChangeLevelButton->OnClicked.AddDynamic(this, &UJolfPauseMenu::OnChangeLevelClicked);

	if (ensure(GameRulesButton))
		GameRulesButton->OnClicked.AddDynamic(this, &UJolfPauseMenu::OnGameRulesClicked);

	if (ensure(SystemSettingsButton))
		SystemSettingsButton->OnClicked.AddDynamic(this, &UJolfPauseMenu::OnSystemSettingsClicked);

	if (ensure(OptionsButton))
		OptionsButton->OnClicked.AddDynamic(this, &UJolfPauseMenu::OnOptionsClicked);

	if (ensure(QuitButton))
		QuitButton->OnClicked.AddDynamic(this, &UJolfPauseMenu::OnQuitClicked);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfPauseMenu::OnResumeClicked()
{
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->ClosePauseMenu();
	}
}

void UJolfPauseMenu::OnMainMenuClicked()
{
	UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance);
	for (int32 LocalPlayerIndex = GameInstance->GetNumLocalPlayers() - 1; LocalPlayerIndex > 0; --LocalPlayerIndex)
	{
		ULocalPlayer* LocalPlayer = GameInstance->GetLocalPlayerByIndex(LocalPlayerIndex);
		check(LocalPlayer);
		GameInstance->RemoveLocalPlayer(LocalPlayer);
	}

	UJolfViewportClient* ViewportClient = CastChecked<UJolfViewportClient>(GameInstance->GetGameViewportClient());
	ViewportClient->bHotseatMode = false;
	ViewportClient->SetHotseatIndex(INDEX_NONE);

	GEngine->SetClientTravel(GetWorld(), TEXT("Startup"), TRAVEL_Absolute);
}

void UJolfPauseMenu::OnRestartHoleClicked()
{
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->ServerRestartHole();
	}
}

void UJolfPauseMenu::OnChangeLevelClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("HostChangeLevel"));
}

void UJolfPauseMenu::OnGameRulesClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("AuthorityRules"));
}

void UJolfPauseMenu::OnSystemSettingsClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("SystemSettings"));
}

void UJolfPauseMenu::OnOptionsClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("Options"));
}

void UJolfPauseMenu::OnQuitClicked()
{
	FPlatformMisc::RequestExit(false);
}