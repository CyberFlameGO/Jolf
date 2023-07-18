// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Engine/LocalPlayer.h"
#include "JolfLocalPlayer.generated.h"

UCLASS()
class JOLF_API UJolfLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public: // Functions

	TSharedPtr<class SWidget> GetSlateUserFocus() const;
	
	/** Game replacement for UWidget::SetUserFocus because in 4.26.1 there is a bug with remapped controller ids. */
	void SetSlateUserFocus(class UWidget* InWidget);
	void SetSlateUserFocus(TSharedPtr<class SWidget> InWidget);

	void MoveCursorToViewportCenter();

	class UJolfPerPlayerSettings* GetPlayerSettings() const;
	void SetPlayerSettings(class UJolfPerPlayerSettings* InPlayerSettings);

public: // Interfaces

	//~ Begin ULocalPlayer Interface
	void PlayerAdded(class UGameViewportClient* InViewportClient, int32 InControllerID) override;
	void PlayerRemoved() override;
	FString GetNickname() const override;
	//~ End ULocalPlayer Interface

public: // Properties

	DECLARE_EVENT_OneParam(UJolfLocalPlayer, FOnPlayerSettingsChangedEvent, class UJolfPerPlayerSettings*);
	FOnPlayerSettingsChangedEvent OnPlayerSettingsChanged;

private: // Functions

	void OnPlayerNameChanged(class UJolfPerPlayerSettings* InPlayerSettings);

private: // Properties

	UPROPERTY()
	class UJolfPerPlayerSettings* PlayerSettings;

	FDelegateHandle NameChangedHandle;
};