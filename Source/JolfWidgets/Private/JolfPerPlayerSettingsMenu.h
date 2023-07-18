// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfPerPlayerSettingsMenu.generated.h"

UCLASS(Abstract)
class UJolfPerPlayerSettingsMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()


protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) BackButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	void SynchronizeSettings();

	void OnPlayerSettingsClicked(class UJolfPerPlayerSettings* InPlayerSettings);
	void OnPlayerSettingsSelected(class UJolfPerPlayerSettings* InPlayerSettings);
	void OnPlayerSettingsChanged(class UJolfPerPlayerSettings* InPlayerSettings);

	UFUNCTION()
	void OnApplyClicked();
	
	UFUNCTION()
	void OnBackClicked();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UJolfLocalPlayerEntry* PlayerWidget;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* MousePitchSensitivitySpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* MouseYawSensitivitySpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* MousePowerSensitivitySpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* GamepadPitchDegreesPerSecondSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* GamepadYawDegreesPerSecondSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* GamepadPowerPerSecondSpinBox;
	
	UPROPERTY(Meta = (BindWidget))
	class USpinBox* HorizontalFieldOfViewSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* ShowStrokePowerPercentageCheckBox;

	UPROPERTY(Meta = (BindWidget))
	class UButton* ApplyButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;

	FDelegateHandle PlayerSettingsChangedHandle;
};