// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPerPlayerSettingsMenu.h"
#include "JolfLocalPlayer.h"
#include "JolfPerPlayerSettings.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "JolfMenuStackRoot.h"
#include "JolfLocalPlayerEntry.h"
#include "JolfChangePlayerSettingsMenu.h"
#include "Components/CheckBox.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfPerPlayerSettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (ensure(PlayerWidget))
		PlayerWidget->OnPlayerSettingsClicked.AddUObject(this, &UJolfPerPlayerSettingsMenu::OnPlayerSettingsClicked);

	if (ensure(ApplyButton))
		ApplyButton->OnClicked.AddDynamic(this, &UJolfPerPlayerSettingsMenu::OnApplyClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfPerPlayerSettingsMenu::OnBackClicked);
}

void UJolfPerPlayerSettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	UJolfLocalPlayer* LocalPlayer = GetOwningLocalPlayer<UJolfLocalPlayer>();
	check(LocalPlayer);
	PlayerSettingsChangedHandle = LocalPlayer->OnPlayerSettingsChanged.AddUObject(this, &UJolfPerPlayerSettingsMenu::OnPlayerSettingsChanged);

	SynchronizeSettings();
}

void UJolfPerPlayerSettingsMenu::NativeDestruct()
{
	// LP is null during end PIE.
	if (UJolfLocalPlayer* LocalPlayer = GetOwningLocalPlayer<UJolfLocalPlayer>())
	{
		LocalPlayer->OnPlayerSettingsChanged.Remove(PlayerSettingsChangedHandle);
		PlayerSettingsChangedHandle.Reset();
	}

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfPerPlayerSettingsMenu::SynchronizeSettings()
{
	UJolfPerPlayerSettings* Settings = GetOwningLocalPlayer<UJolfLocalPlayer>()->GetPlayerSettings();

	if (ensure(PlayerWidget))
		PlayerWidget->SetPlayerSettings(Settings);

	if (ensure(MousePitchSensitivitySpinBox))
		MousePitchSensitivitySpinBox->SetValue(Settings->MousePitchSensitivity);

	if (ensure(MouseYawSensitivitySpinBox))
		MouseYawSensitivitySpinBox->SetValue(Settings->MouseYawSensitivity);

	if (ensure(MousePowerSensitivitySpinBox))
		MousePowerSensitivitySpinBox->SetValue(Settings->MousePowerSensitivity * 100.f);

	if (ensure(GamepadPitchDegreesPerSecondSpinBox))
		GamepadPitchDegreesPerSecondSpinBox->SetValue(Settings->GamepadPitchDegreesPerSecond);

	if (ensure(GamepadYawDegreesPerSecondSpinBox))
		GamepadYawDegreesPerSecondSpinBox->SetValue(Settings->GamepadYawDegreesPerSecond);

	if (ensure(GamepadPowerPerSecondSpinBox))
		GamepadPowerPerSecondSpinBox->SetValue(Settings->GamepadPowerSensitivity * 100.0f);

	if (ensure(HorizontalFieldOfViewSpinBox))
		HorizontalFieldOfViewSpinBox->SetValue(Settings->HorizontalFieldOfView);

	if (ensure(ShowStrokePowerPercentageCheckBox))
		ShowStrokePowerPercentageCheckBox->SetIsChecked(Settings->bShowStrokePowerPercentage);
}

void UJolfPerPlayerSettingsMenu::OnPlayerSettingsClicked(UJolfPerPlayerSettings* InPlayerSettings)
{
	UJolfChangePlayerSettingsMenu* ChangeMenu = GetMenuRoot()->FindMenuByClass<UJolfChangePlayerSettingsMenu>();
	check(ChangeMenu);
	ChangeMenu->SetActivePlayerSettings(InPlayerSettings);
	ChangeMenu->OnSelectionChanged.BindUObject(this, &UJolfPerPlayerSettingsMenu::OnPlayerSettingsSelected);
	GetMenuRoot()->SetActiveMenu(TEXT("ChangePlayerSettings"));
}

void UJolfPerPlayerSettingsMenu::OnPlayerSettingsSelected(UJolfPerPlayerSettings* InPlayerSettings)
{
	UJolfLocalPlayer* LocalPlayer = GetOwningLocalPlayer<UJolfLocalPlayer>();
	check(LocalPlayer);
	LocalPlayer->SetPlayerSettings(InPlayerSettings);
}

void UJolfPerPlayerSettingsMenu::OnPlayerSettingsChanged(class UJolfPerPlayerSettings* InPlayerSettings)
{
	if (ensure(PlayerWidget))
		PlayerWidget->SetPlayerSettings(InPlayerSettings);
}

void UJolfPerPlayerSettingsMenu::OnApplyClicked()
{
	UJolfPerPlayerSettings* Settings = GetOwningLocalPlayer<UJolfLocalPlayer>()->GetPlayerSettings();

	if (ensure(MousePitchSensitivitySpinBox))
		Settings->MousePitchSensitivity = MousePitchSensitivitySpinBox->GetValue();

	if (ensure(MouseYawSensitivitySpinBox))
		Settings->MouseYawSensitivity = MouseYawSensitivitySpinBox->GetValue();

	if (ensure(MousePowerSensitivitySpinBox))
		Settings->MousePowerSensitivity = MousePowerSensitivitySpinBox->GetValue() * 0.01f;

	if (ensure(GamepadPitchDegreesPerSecondSpinBox))
		Settings->GamepadPitchDegreesPerSecond = GamepadPitchDegreesPerSecondSpinBox->GetValue();

	if (ensure(GamepadYawDegreesPerSecondSpinBox))
		Settings->GamepadYawDegreesPerSecond = GamepadYawDegreesPerSecondSpinBox->GetValue();

	if (ensure(GamepadPowerPerSecondSpinBox))
		Settings->GamepadPowerSensitivity = GamepadPowerPerSecondSpinBox->GetValue() * 0.01f;

	if (ensure(HorizontalFieldOfViewSpinBox))
		Settings->HorizontalFieldOfView = HorizontalFieldOfViewSpinBox->GetValue();

	if (ensure(ShowStrokePowerPercentageCheckBox))
		Settings->bShowStrokePowerPercentage = ShowStrokePowerPercentageCheckBox->IsChecked();

	Settings->SaveSettings();
}

void UJolfPerPlayerSettingsMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}