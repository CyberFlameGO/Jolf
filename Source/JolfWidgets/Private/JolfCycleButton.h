// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfCycleButton.generated.h"

UCLASS()
class UJolfCycleButton : public UUserWidget
{
	GENERATED_BODY()

public: // Properties

	UPROPERTY(EditAnywhere, Category = "Content")
	TArray<FText> Options;

	DECLARE_EVENT_OneParam(UJolfCycleButton, FOnSelectionChangedEvent, int32);
	FOnSelectionChangedEvent OnSelectionChanged;

public: // Functions

	int32 GetSelectedIndex() const;
	void SetSelectedIndex(int32 InIndex);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnButtonClicked();

private: // Properties

	UPROPERTY()
	int32 SelectedIndex;

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;
	
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* Label;
};