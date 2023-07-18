// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfCreditsButton.generated.h"

UCLASS(Abstract)
class UJolfCreditsButton : public UUserWidget
{
	GENERATED_BODY()

public: // Properties

	UPROPERTY(EditAnywhere, Meta = (MultiLine = "true"))
	FText ButtonText;

	UPROPERTY(EditAnywhere)
	FString LinkUrl;

public: // Interfaces

	//~ Begin UWidget Interface
	void SynchronizeProperties() override;
	//~ End UWidget Interface

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnButtonClicked();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* TextBlock;
};