// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfMainMenu.generated.h"

UCLASS(Abstract)
class UJolfMainMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) SingleplayerButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	UFUNCTION()
	void OnSingleplayerClicked();
	
	UFUNCTION()
	void OnSplitscreenClicked();

	UFUNCTION()
	void OnHotseatClicked();

	UFUNCTION()
	void OnHostClicked();

	UFUNCTION()
	void OnJoinClicked();

	UFUNCTION()
	void OnSystemSettingsClicked();

	UFUNCTION()
	void OnOptionsClicked();

	UFUNCTION()
	void OnQuitClicked();

	UFUNCTION()
	void OnCreditsClicked();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UButton* SingleplayerButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* SplitscreenButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* HotseatButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* SystemSettingsButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* OptionsButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* QuitButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* CreditsButton;
};