// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfCreditsMenu.generated.h"

UCLASS(Abstract)
class UJolfCreditsMenu : public UJolfMenuStackContent
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
	void OnBackClicked();

private: // Properties
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;
};