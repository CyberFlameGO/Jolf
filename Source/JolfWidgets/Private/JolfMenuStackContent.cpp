// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfMenuStackContent.h"
#include "JolfMenuStackRoot.h"

UJolfMenuStackContent::UJolfMenuStackContent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsFocusable = true;
}

// Public Functions:

UJolfMenuStackRoot* UJolfMenuStackContent::GetMenuRoot() const
{
	return WeakRoot.Get();
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
FReply UJolfMenuStackContent::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (UWidget* WidgetToFocus = GetDefaultWidgetToFocus())
	{
		return FReply::Handled().SetUserFocus(WidgetToFocus->TakeWidget(), InFocusEvent.GetCause(), false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Menu %s null DefaultWidgetToFocus"), *GetName());
		return FReply::Unhandled();
	}
}
//~ End UUserWidget Interface