// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfHoleScore.generated.h"

UCLASS(Abstract)
class UJolfHoleScore : public UUserWidget
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	//~ End UUserWidget Interface

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* TitleTextBlock;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* DescriptionTextBlock;
	
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* TimeTextBlock;

	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	class UWidgetAnimation* PopupAnim;
};