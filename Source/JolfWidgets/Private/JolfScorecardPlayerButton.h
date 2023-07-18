// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfScorecardPlayerButton.generated.h"

UCLASS(Abstract)
class UJolfScorecardPlayerButton : public UUserWidget
{
	GENERATED_BODY()

public:

	UJolfScorecardPlayerButton(const FObjectInitializer& ObjectInitializer);

public: // Functions

	class AJolfPlayerState* GetPlayerState() const { return PlayerState; }
	void SetPlayerState(class AJolfPlayerState* InPlayerState);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeDestruct() override;
	FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	//~ End UUserWidget Interface

private: // Functions

	void OnPlayerNameChanged(class AJolfPlayerState* InPlayerState);

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* PlayerNameTextBlock;

	UPROPERTY(Transient)
	class AJolfPlayerState* PlayerState;

	FDelegateHandle OnPlayerNameChangedEventHandle;
};