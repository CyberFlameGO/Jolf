// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSlateNavigationConfig.h"
#include "JolfSlateFocus.h"
#include "JolfViewportClient.h"
#include "Engine/Engine.h"

// Public Interfaces:

//~ Begin FNavigationConfig Interface
void FJolfSlateNavigationConfig::OnNavigationChangedFocus(TSharedPtr<SWidget> OldWidget, TSharedPtr<SWidget> NewWidget, FFocusEvent FocusEvent)
{
	FNavigationConfig::OnNavigationChangedFocus(OldWidget, NewWidget, FocusEvent);

	if (UJolfViewportClient* Viewport = Cast<UJolfViewportClient>(GEngine->GameViewport))
	{
		if (Viewport->GetHotseatIndex() != INDEX_NONE)
		{
			if (FocusEvent.GetCause() == EFocusCause::Mouse)
			{
				FJolfSlateFocus::bDrawMouseHover = true;
				FJolfSlateFocus::DrawFocusHoverMask &= ~(1 << FocusEvent.GetUser());
			}
			else if(FocusEvent.GetCause() == EFocusCause::Navigation)
			{
				FJolfSlateFocus::bDrawMouseHover = false;
				FJolfSlateFocus::DrawFocusHoverMask |= (1 << FocusEvent.GetUser());
			}
			return;
		}
	}

	if (FocusEvent.GetUser() == 0)
	{
		if (FocusEvent.GetCause() == EFocusCause::Mouse)
		{
			FJolfSlateFocus::bDrawMouseHover = true;
			FJolfSlateFocus::DrawFocusHoverMask &= ~(1 << FocusEvent.GetUser());
		}
		else if (FocusEvent.GetCause() == EFocusCause::Navigation)
		{
			FJolfSlateFocus::bDrawMouseHover = false;
			FJolfSlateFocus::DrawFocusHoverMask |= (1 << FocusEvent.GetUser());
		}
	}
	else
	{
		FJolfSlateFocus::DrawFocusHoverMask |= (1 << FocusEvent.GetUser());
	}
}
//~ End FNavigationConfig Interface