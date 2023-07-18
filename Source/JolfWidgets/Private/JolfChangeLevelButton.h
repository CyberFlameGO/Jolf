// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfChangeLevelButton.generated.h"

UCLASS(Abstract)
class UJolfChangeLevelButton : public UUserWidget
{
	GENERATED_BODY()

public: // Functions
	
	class UJolfLevelDefinition* GetLevelDef() const { return LevelDef; }
	void SetLevelDef(class UJolfLevelDefinition* InLevelDef);

public: // Properties

	FSimpleMulticastDelegate OnClicked;

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnButtonClicked();

private: // Properties

	UPROPERTY(Transient)
	class UJolfLevelDefinition* LevelDef;

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;

	UPROPERTY(Meta = (BindWidget))
	class UImage* ThumbnailImage;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* TitleTextBlock;
};