// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "SJolfButton.h"
#include "JolfSlateFocus.h"

// Public Interfaces:

//~ Begin SWidget Interface
void SJolfButton::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	SButton::OnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	TSharedRef<SWidget> SharedThis = AsShared();
	const bool bDecendantNewlyFocused = NewWidgetPath.ContainsWidget(SharedThis);
	if (bDecendantNewlyFocused)
	{
		if (InFocusEvent.GetCause() != EFocusCause::Mouse)
		{
			UserFocusHoverMask |= (1 << InFocusEvent.GetUser());
		}
	}
	else
	{
		UserFocusHoverMask &= ~(1 << InFocusEvent.GetUser());
	}
}

bool SJolfButton::IsHovered() const
{
	return (SButton::IsHovered() & FJolfSlateFocus::bDrawMouseHover) || (UserFocusHoverMask & FJolfSlateFocus::DrawFocusHoverMask);
}
//~ End SWidget Interface