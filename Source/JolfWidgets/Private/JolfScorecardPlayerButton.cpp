// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfScorecardPlayerButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "JolfPlayerState.h"

UJolfScorecardPlayerButton::UJolfScorecardPlayerButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsFocusable = true;
}

// Public Functions:

void UJolfScorecardPlayerButton::SetPlayerState(AJolfPlayerState* InPlayerState)
{
	check(PlayerState == nullptr);
	PlayerState = InPlayerState;

	if (PlayerState)
	{
		OnPlayerNameChangedEventHandle = PlayerState->OnPlayerNameChanged.AddUObject(this, &UJolfScorecardPlayerButton::OnPlayerNameChanged);
		OnPlayerNameChanged(InPlayerState);
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfScorecardPlayerButton::NativeDestruct()
{
	if (PlayerState)
	{
		PlayerState->OnPlayerNameChanged.Remove(OnPlayerNameChangedEventHandle);
		OnPlayerNameChangedEventHandle.Reset();

		PlayerState = nullptr;
	}

	Super::NativeDestruct();
}

FReply UJolfScorecardPlayerButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (Button)
	{
		return FReply::Handled().SetUserFocus(Button->TakeWidget(), InFocusEvent.GetCause(), false);
	}
	else
	{
		return FReply::Unhandled();
	}
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfScorecardPlayerButton::OnPlayerNameChanged(AJolfPlayerState* InPlayerState)
{
	if (ensure(PlayerNameTextBlock))
		PlayerNameTextBlock->SetText(FText::AsCultureInvariant(InPlayerState->GetPlayerName()));
}