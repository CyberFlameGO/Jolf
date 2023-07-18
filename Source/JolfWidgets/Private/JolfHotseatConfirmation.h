// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfHotseatConfirmation.generated.h"

UCLASS(Abstract)
class UJolfHotseatConfirmation : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UJolfHotseatConfirmation(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	//~ End UUserWidget Interface

private: // Functions

	void Confirm();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* PlayerNameTextBlock;
};