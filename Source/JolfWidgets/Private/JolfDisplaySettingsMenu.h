// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfDisplaySettingsMenu.generated.h"

/** Settings related to the monitor/display e.g. resolution and fullscreen. */
UCLASS(Abstract)
class UJolfDisplaySettingsMenu : public UJolfMenuStackContent
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

	void OnResolutionClicked(const FIntPoint& InResolution);

private: // Properties
	
	UPROPERTY(Meta = (BindWidget))
	class USpinBox* WidthSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* HeightSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class UJolfCycleButton* FullscreenModeComboBox;

	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* VSyncCheckBox;

	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* UseFrameRateLimitCheckBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* FrameRateLimitSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* UserInterfaceScaleSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class UScrollBox* ResolutionScrollBox;

	UPROPERTY(Meta = (BindWidget))
	class UButton* ApplyButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UJolfDisplayResolutionEntry> ResolutionEntryClass;
};