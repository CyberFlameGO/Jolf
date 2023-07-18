// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfLocalPlayerEntry.generated.h"

UCLASS(Abstract)
class UJolfLocalPlayerEntry : public UUserWidget
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
	FOnPlayerSettingsClickedEvent OnPlayerSettingsClicked;

private: // Functions

	UFUNCTION()
	void OnClicked();

	void OnPlayerNameChanged(class UJolfPerPlayerSettings* InPlayerSettings);

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* PlayerNameTextBlock;

	UPROPERTY(Transient)
	class UJolfPerPlayerSettings* PlayerSettings;

	FDelegateHandle OnPlayerNameChangedHandle;
};