// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once


#include "Blueprint/UserWidget.h"
#include "JolfChatWidget.generated.h"

UCLASS(Abstract)
class UJolfChatWidget : public UUserWidget
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
	//~ End UUserWidget Interface

private: // Functions

	void OnTextChatRequested();

	UFUNCTION()
	void OnTextChatCommitted(const FText& InText, ETextCommit::Type InCommitMethod);

	UFUNCTION()
	void OnTextChatReceived(class AJolfPlayerState* InSpeaker, const FText& InText);

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UJolfEditableTextBox* TextChatField;

	UPROPERTY(Meta = (BindWidget))
	class UScrollBox* TextChatHistory;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UJolfTextChatEntry> TextChatEntryClass;
};