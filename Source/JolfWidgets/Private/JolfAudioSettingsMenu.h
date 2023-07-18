// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfAudioSettingsMenu.generated.h"

UCLASS(Abstract)
class UJolfAudioSettingsMenu : public UJolfMenuStackContent
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
	class UJolfCycleButton* QualityComboBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* VolumeMultiplierSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* UnfocusedVolumeMultiplierSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* MusicVolumeMultiplierSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class UButton* ApplyButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;
};