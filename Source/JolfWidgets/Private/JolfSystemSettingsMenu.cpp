// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSystemSettingsMenu.h"
#include "Components/Button.h"
#include "JolfMenuStackRoot.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfSystemSettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(AudioSettingsButton))
		AudioSettingsButton->OnClicked.AddDynamic(this, &UJolfSystemSettingsMenu::OnAudioSettingsClicked);

	if (ensure(DisplaySettingsButton))
		DisplaySettingsButton->OnClicked.AddDynamic(this, &UJolfSystemSettingsMenu::OnDisplaySettingsClicked);

	if (ensure(RenderSettingsButton))
		RenderSettingsButton->OnClicked.AddDynamic(this, &UJolfSystemSettingsMenu::OnRenderSettingsClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfSystemSettingsMenu::OnBackClicked);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfSystemSettingsMenu::OnAudioSettingsClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("AudioSettings"));
}

void UJolfSystemSettingsMenu::OnDisplaySettingsClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("DisplaySettings"));
}

void UJolfSystemSettingsMenu::OnRenderSettingsClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("RenderSettings"));
}

void UJolfSystemSettingsMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}