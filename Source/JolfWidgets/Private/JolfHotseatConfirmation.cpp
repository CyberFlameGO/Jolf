// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfHotseatConfirmation.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "JolfPlayerController.h"

UJolfHotseatConfirmation::UJolfHotseatConfirmation(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsFocusable = true;
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfHotseatConfirmation::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ensure(PlayerNameTextBlock))
	{
		APlayerState* PlayerState = GetOwningPlayerState();
		if (ensure(PlayerState))
		{
			PlayerNameTextBlock->SetText(FText::AsCultureInvariant(PlayerState->GetPlayerName()));
		}
	}
}

FReply UJolfHotseatConfirmation::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// Wait until key up to prevent game from handling it.
	return FReply::Handled();
}

FReply UJolfHotseatConfirmation::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();
	// Ignore analog "key up"
	if (Key != EKeys::Gamepad_RightStick_Up &&
		Key != EKeys::Gamepad_RightStick_Down &&
		Key != EKeys::Gamepad_RightStick_Right &&
		Key != EKeys::Gamepad_RightStick_Left &&
		Key != EKeys::Gamepad_LeftStick_Up &&
		Key != EKeys::Gamepad_LeftStick_Down &&
		Key != EKeys::Gamepad_LeftStick_Right &&
		Key != EKeys::Gamepad_LeftStick_Left)
	{
		Confirm();
	}
	
	return FReply::Handled();
}

FReply UJolfHotseatConfirmation::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Wait until mouse up to prevent game from handling it.
	return FReply::Handled();
}

FReply UJolfHotseatConfirmation::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Confirm();
	return FReply::Handled();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfHotseatConfirmation::Confirm()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->ConfirmHotseat();
	}
}