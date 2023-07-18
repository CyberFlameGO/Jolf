// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfTextChatEntry.generated.h"

UCLASS(Abstract)
class UJolfTextChatEntry : public UUserWidget
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	//~ End UUserWidget Interface

public: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* MessageTextBlock;

	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	class UWidgetAnimation* FadeOut;
};