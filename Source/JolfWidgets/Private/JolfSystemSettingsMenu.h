// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfSystemSettingsMenu.generated.h"

UCLASS(Abstract)
class UJolfSystemSettingsMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) BackButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	UFUNCTION()
	void OnDisplaySettingsClicked();

	UFUNCTION()
	void OnAudioSettingsClicked();

	UFUNCTION()
	void OnRenderSettingsClicked();

	UFUNCTION()
	void OnBackClicked();

private: // Properties
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* AudioSettingsButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* DisplaySettingsButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* RenderSettingsButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;
};