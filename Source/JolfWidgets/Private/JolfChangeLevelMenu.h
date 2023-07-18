// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfChangeLevelMenu.generated.h"

UCLASS(Abstract)
class UJolfChangeLevelMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()
	
protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) ChangeLevelButton; }
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