// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfSingleplayerMenu.generated.h"

UCLASS(Abstract)
class UJolfSingleplayerMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()
	
protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) TravelButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	UFUNCTION()
	void OnChangeLevelClicked();

	UFUNCTION()
	void OnTravelClicked();

	UFUNCTION()
	void OnBackClicked();

	void OnLevelSelectionChanged(class UJolfLevelDefinition* InLevelDef);

private: // Properties
	
	UPROPERTY(Meta = (BindWidget))
	class UJolfChangeLevelButton* ChangeLevelButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* TravelButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;
};