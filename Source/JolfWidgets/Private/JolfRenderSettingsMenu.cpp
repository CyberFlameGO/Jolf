// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfRenderSettingsMenu.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "JolfUserSettings.h"
#include "Components/CheckBox.h"
#include "JolfMenuStackRoot.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfRenderSettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(ApplyButton))
		ApplyButton->OnClicked.AddDynamic(this, &UJolfRenderSettingsMenu::OnApplyClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfRenderSettingsMenu::OnBackClicked);
}

void UJolfRenderSettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	SynchronizeSettings();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfRenderSettingsMenu::SynchronizeSettings()
{
	const UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();

	if (ensure(ResolutionScaleSpinBox))
	{
		float CurrentScaleNormalized, CurrentScaleValue, MinScaleValue, MaxScaleValue;
		Settings->GetResolutionScaleInformationEx(CurrentScaleNormalized, CurrentScaleValue, MinScaleValue, MaxScaleValue);

		ResolutionScaleSpinBox->SetMinValue(MinScaleValue);
		ResolutionScaleSpinBox->SetMinSliderValue(MinScaleValue);
		ResolutionScaleSpinBox->SetMaxValue(MaxScaleValue);
		ResolutionScaleSpinBox->SetMaxSliderValue(MaxScaleValue);
		ResolutionScaleSpinBox->SetValue(CurrentScaleValue);
	}

	if (ensure(SceneProbeCheckBox))
		SceneProbeCheckBox->SetIsChecked(Settings->bEnableSceneProbe);

	if (ensure(ShadowQualityComboBox))
		ShadowQualityComboBox->SetSelectedIndex(Settings->GetShadowQuality());

	if (ensure(AntiAliasingQualityComboBox))
		AntiAliasingQualityComboBox->SetSelectedIndex(Settings->GetAntiAliasingQuality());

	if (ensure(TextureQualityComboBox))
		TextureQualityComboBox->SetSelectedIndex(Settings->GetTextureQuality());

	if (ensure(EffectQualityComboBox))
		EffectQualityComboBox->SetSelectedIndex(Settings->GetVisualEffectQuality());

	if (ensure(PostProcessingQualityComboBox))
		PostProcessingQualityComboBox->SetSelectedIndex(Settings->GetPostProcessingQuality());

	if (ensure(FoliageQualityComboBox))
		FoliageQualityComboBox->SetSelectedIndex(Settings->GetFoliageQuality());

	if (ensure(ShadingQualityComboBox))
		ShadingQualityComboBox->SetSelectedIndex(Settings->GetShadingQuality());
}

void UJolfRenderSettingsMenu::OnApplyClicked()
{
	UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();
	
	if (ensure(ResolutionScaleSpinBox))
		Settings->SetResolutionScaleValueEx(ResolutionScaleSpinBox->GetValue());

	if (ensure(SceneProbeCheckBox))
		Settings->bEnableSceneProbe = SceneProbeCheckBox->IsChecked();

	if (ensure(ShadowQualityComboBox))
		Settings->SetShadowQuality(ShadowQualityComboBox->GetSelectedIndex());

	if (ensure(AntiAliasingQualityComboBox))
		Settings->SetAntiAliasingQuality(AntiAliasingQualityComboBox->GetSelectedIndex());

	if (ensure(TextureQualityComboBox))
		Settings->SetTextureQuality(TextureQualityComboBox->GetSelectedIndex());

	if (ensure(EffectQualityComboBox))
		Settings->SetVisualEffectQuality(EffectQualityComboBox->GetSelectedIndex());

	if (ensure(PostProcessingQualityComboBox))
		Settings->SetPostProcessingQuality(PostProcessingQualityComboBox->GetSelectedIndex());

	if (ensure(FoliageQualityComboBox))
		Settings->SetFoliageQuality(FoliageQualityComboBox->GetSelectedIndex());

	if (ensure(ShadingQualityComboBox))
		Settings->SetShadingQuality(ShadingQualityComboBox->GetSelectedIndex());

	Settings->ApplySettings(true);
}

void UJolfRenderSettingsMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}