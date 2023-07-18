// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfVideoModePrompt.generated.h"

UCLASS()
class UJolfVideoModePrompt : public UJolfMenuStackContent
{
	GENERATED_BODY()

public: // Functions

	void ResetPrompt(FSimpleDelegate InCallback);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*) RevertButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnRevertClicked();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* TimerLabel;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* ConfirmButton;
	
	UPROPERTY(Meta = (BindWidget))
	class UButton* RevertButton;

	UPROPERTY()
	float RemainingTime;

	UPROPERTY()
	bool bDecisionPending;

	FSimpleDelegate DecisionCallback;
};