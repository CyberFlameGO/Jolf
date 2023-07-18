// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfAudioSettingsMenu.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "JolfMenuStackRoot.h"
#include "JolfUserSettings.h"
#include "JolfCycleButton.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfAudioSettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (ensure(ApplyButton))
		ApplyButton->OnClicked.AddDynamic(this, &UJolfAudioSettingsMenu::OnApplyClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfAudioSettingsMenu::OnBackClicked);
}

void UJolfAudioSettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	SynchronizeSettings();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfAudioSettingsMenu::SynchronizeSettings()
{
	UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();

	if (ensure(QualityComboBox))
		QualityComboBox->SetSelectedIndex(Settings->GetAudioQualityLevel());

	if (ensure(VolumeMultiplierSpinBox))
		VolumeMultiplierSpinBox->SetValue(Settings->VolumeMultiplier * 100.f);

	if (ensure(UnfocusedVolumeMultiplierSpinBox))
		UnfocusedVolumeMultiplierSpinBox->SetValue(Settings->UnfocusedVolumeMultiplier * 100.f);

	if (ensure(MusicVolumeMultiplierSpinBox))
		MusicVolumeMultiplierSpinBox->SetValue(Settings->MusicVolumeMultiplier * 100.f);
}

void UJolfAudioSettingsMenu::OnApplyClicked()
{
	UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();

	if (ensure(QualityComboBox))
		Settings->SetAudioQualityLevel(QualityComboBox->GetSelectedIndex());

	if (ensure(VolumeMultiplierSpinBox))
		Settings->VolumeMultiplier = VolumeMultiplierSpinBox->GetValue() * 0.01f;

	if (ensure(UnfocusedVolumeMultiplierSpinBox))
		Settings->UnfocusedVolumeMultiplier = UnfocusedVolumeMultiplierSpinBox->GetValue() * 0.01f;

	if (ensure(MusicVolumeMultiplierSpinBox))
		Settings->MusicVolumeMultiplier = MusicVolumeMultiplierSpinBox->GetValue() * 0.01f;

	Settings->ApplyNonResolutionSettings();
	Settings->SaveSettings();
}

void UJolfAudioSettingsMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}