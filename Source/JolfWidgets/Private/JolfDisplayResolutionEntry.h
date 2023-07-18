// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfDisplayResolutionEntry.generated.h"

UCLASS(Abstract)
class UJolfDisplayResolutionEntry : public UUserWidget
{
	GENERATED_BODY()

public: // Functions
	
	void SetResolution(const FIntPoint& InResolution);

public: // Properties

	DECLARE_EVENT_OneParam(UJolfDisplayResolutionEntry, FOnClicked, const FIntPoint&);
	FOnClicked OnClicked;

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnButtonClicked();

private: // Properties

	UPROPERTY(Transient)
	FIntPoint Resolution;

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;
	
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* ResolutionTextBox;
	
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* AspectRatioTextBox;
};