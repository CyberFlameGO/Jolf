// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfManagePlayerSettingsEntry.generated.h"

UCLASS(Abstract)
class UJolfManagePlayerSettingsEntry : public UUserWidget
{
	GENERATED_BODY()

public: // Functions

	class UJolfPerPlayerSettings* GetPlayerSettings() const { return PlayerSettings; }
	void SetPlayerSettings(class UJolfPerPlayerSettings* InPlayerSettings);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeDestruct() override;
	//~ End UUserWidget Interface

public: // Properties

	DECLARE_EVENT_OneParam(UJolfLocalPlayerEntry, FOnPlayerSettingsClickedEvent, class UJolfPerPlayerSettings*);
	FOnPlayerSettingsClickedEvent OnSelectClicked;
	FOnPlayerSettingsClickedEvent OnRemoveClicked;

private: // Functions

	UFUNCTION()
	void OnSelectButtonClicked();
	
	UFUNCTION()
	void OnRemoveButtonClicked();

	void OnPlayerNameChanged(class UJolfPerPlayerSettings* InPlayerSettings);
	void OnNumUsersChanged(class UJolfPerPlayerSettings* InPlayerSettings);

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UButton* SelectButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* RemoveButton;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* PlayerNameTextBlock;

	UPROPERTY(Transient)
	class UJolfPerPlayerSettings* PlayerSettings;

	FDelegateHandle OnPlayerNameChangedHandle;
	FDelegateHandle OnNumUsersChangedHandle;
};