// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfRenderSettingsMenu.generated.h"

UCLASS(Abstract)
class UJolfRenderSettingsMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) BackButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	void SynchronizeSettings();
	
	UFUNCTION()
	void OnApplyClicked();
	
	UFUNCTION()
	void OnBackClicked();

private: // Properties
	
	UPROPERTY(Meta = (BindWidget))
	class USpinBox* ResolutionScaleSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* SceneProbeCheckBox;

	UPROPERTY(Meta = (BindWidget))
	class UJolfCycleButton* ShadowQualityComboBox;

	UPROPERTY(Meta = (BindWidget))
	class UJolfCycleButton* AntiAliasingQualityComboBox;

	UPROPERTY(Meta = (BindWidget))
	class UJolfCycleButton* TextureQualityComboBox;

	UPROPERTY(Meta = (BindWidget))
	class UJolfCycleButton* EffectQualityComboBox;

	UPROPERTY(Meta = (BindWidget))
	class UJolfCycleButton* PostProcessingQualityComboBox;

	UPROPERTY(Meta = (BindWidget))
	class UJolfCycleButton* FoliageQualityComboBox;
	
	UPROPERTY(Meta = (BindWidget))
	class UJolfCycleButton* ShadingQualityComboBox;

	UPROPERTY(Meta = (BindWidget))
	class UButton* ApplyButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;
};