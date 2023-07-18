// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfPauseMenu.generated.h"

UCLASS(Abstract)
class UJolfPauseMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) ResumeButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void OnMainMenuClicked();

	UFUNCTION()
	void OnRestartHoleClicked();

	UFUNCTION()
	void OnChangeLevelClicked();
	
	UFUNCTION()
	void OnGameRulesClicked();

	UFUNCTION()
	void OnSystemSettingsClicked();

	UFUNCTION()
	void OnOptionsClicked();

	UFUNCTION()
	void OnQuitClicked();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UButton* ResumeButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* MainMenuButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* RestartHoleButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* ChangeLevelButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* GameRulesButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* SystemSettingsButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* OptionsButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* QuitButton;
};