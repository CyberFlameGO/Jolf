// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfLevelsMenu.generated.h"

UCLASS(Abstract)
class UJolfLevelsMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()

public: // Properties

	DECLARE_EVENT_OneParam(UJolfLevelsMenu, FOnSelectionChangedEvent, class UJolfLevelDefinition*);
	FOnSelectionChangedEvent OnSelectionChanged;

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override;
	//~ End UJolfMenuStackContent Interface

private: // Functions

	UFUNCTION()
	void OnBackClicked();

	void OnLevelClicked(class UJolfLevelDefinition* InLevelDef);

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UScrollBox* ScrollBox;

	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UJolfLevelEntry> EntryClass;

	UPROPERTY(Transient)
	TArray<class UJolfLevelEntry*> EntryWidgets;
};