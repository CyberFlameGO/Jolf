// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfJoinMenu.generated.h"

UCLASS(Abstract)
class UJolfJoinMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) TravelButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	UFUNCTION()
	void OnTravelClicked();

	UFUNCTION()
	void OnBackClicked();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UButton* TravelButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(Meta = (BindWidget))
	class UJolfEditableTextBox* AddressTextBox;
};