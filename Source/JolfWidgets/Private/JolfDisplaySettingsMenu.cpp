// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfDisplaySettingsMenu.h"
#include "Components/Button.h"
#include "JolfUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CheckBox.h"
#include "JolfMenuStackRoot.h"
#include "JolfCycleButton.h"
#include "JolfVideoModePrompt.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfDisplaySettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<FIntPoint> Resolutions;
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
	if (ensure(ResolutionScrollBox))
	{
		for (const FIntPoint& Resolution : Resolutions)
		{
			UJolfDisplayResolutionEntry* Entry = CreateWidget<UJolfDisplayResolutionEntry>(this, ResolutionEntryClass);
			if (ensure(Entry))
			{
				Entry->SetResolution(Resolution);
				Entry->OnClicked.AddUObject(this, &UJolfDisplaySettingsMenu::OnResolutionClicked);
				ResolutionScrollBox->AddChild(Entry);
			}
		}
	}

	if (ensure(ApplyButton))
		ApplyButton->OnClicked.AddDynamic(this, &UJolfDisplaySettingsMenu::OnApplyClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfDisplaySettingsMenu::OnBackClicked);
}

void UJolfDisplaySettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	SynchronizeSettings();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfDisplaySettingsMenu::SynchronizeSettings()
{
	const UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();

	if (ensure(WidthSpinBox))
		WidthSpinBox->SetValue(Settings->GetScreenResolution().X);

	if (ensure(HeightSpinBox))
		HeightSpinBox->SetValue(Settings->GetScreenResolution().Y);

	if (ensure(FullscreenModeComboBox))
		FullscreenModeComboBox->SetSelectedIndex(Settings->GetFullscreenMode());

	if (ensure(VSyncCheckBox))
		VSyncCheckBox->SetIsChecked(Settings->IsVSyncEnabled());

	if (ensure(UseFrameRateLimitCheckBox))
		UseFrameRateLimitCheckBox->SetIsChecked(Settings->bUseFrameRateLimit);

	if (ensure(FrameRateLimitSpinBox))
		FrameRateLimitSpinBox->SetValue(Settings->GetFrameRateLimit());

	if (ensure(UserInterfaceScaleSpinBox))
		UserInterfaceScaleSpinBox->SetValue(Settings->UserInterfaceScale * 100.f);
}

void UJolfDisplaySettingsMenu::OnApplyClicked()
{
	UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();
	
	if (ensure(WidthSpinBox) && ensure(HeightSpinBox))
	{
		int32 X = FMath::RoundToInt(WidthSpinBox->GetValue());
		int32 Y = FMath::RoundToInt(HeightSpinBox->GetValue());
		Settings->SetScreenResolution(FIntPoint(X, Y));
	}

	if (ensure(FullscreenModeComboBox))
		Settings->SetFullscreenMode((EWindowMode::Type) FullscreenModeComboBox->GetSelectedIndex());

	if (ensure(VSyncCheckBox))
		Settings->SetVSyncEnabled(VSyncCheckBox->IsChecked());

	if (ensure(UseFrameRateLimitCheckBox))
		Settings->bUseFrameRateLimit = UseFrameRateLimitCheckBox->IsChecked();

	if (ensure(FrameRateLimitSpinBox))
		Settings->SetFrameRateLimit(FrameRateLimitSpinBox->GetValue());

	if (ensure(UserInterfaceScaleSpinBox))
		Settings->UserInterfaceScale = UserInterfaceScaleSpinBox->GetValue() * 0.01f;

	Settings->ApplySettings(true);

	bool bShowVideoModePrompt = Settings->GetFullscreenMode() != Settings->GetLastConfirmedFullscreenMode()
		|| Settings->GetScreenResolution() != Settings->GetLastConfirmedScreenResolution()
		|| Settings->UserInterfaceScale != Settings->LastConfirmedUserInterfaceScale;
	if (bShowVideoModePrompt)
	{
		UJolfVideoModePrompt* VideoModePrompt = GetMenuRoot()->FindMenuByClass<UJolfVideoModePrompt>();
		if (ensure(VideoModePrompt))
		{
			VideoModePrompt->ResetPrompt(FSimpleDelegate::CreateUObject(this, &UJolfDisplaySettingsMenu::SynchronizeSettings));
			GetMenuRoot()->SetActiveMenu(TEXT("VideoModePrompt"));
		}
	}
}

void UJolfDisplaySettingsMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfDisplaySettingsMenu::OnResolutionClicked(const FIntPoint& InResolution)
{
	if (ensure(WidthSpinBox))
		WidthSpinBox->SetValue(InResolution.X);

	if (ensure(HeightSpinBox))
		HeightSpinBox->SetValue(InResolution.Y);
}