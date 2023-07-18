// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Widgets/Input/SSpinBox.h"
#include "JolfSlateFocus.h"

template<typename NumericType>
class SJolfSpinBox : public SSpinBox<NumericType>
{
public: // Interfaces

	//~ Begin SWidget Interface
	void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override
	{
		SSpinBox<NumericType>::OnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

		TSharedRef<SWidget> SharedThis = AsShared();
		const bool bDecendantNewlyFocused = NewWidgetPath.ContainsWidget(SharedThis);
		if (bDecendantNewlyFocused)
		{
			UserFocusHoverMask |= (1 << InFocusEvent.GetUser());
		}
		else
		{
			UserFocusHoverMask &= ~(1 << InFocusEvent.GetUser());
		}
	}
	
	bool IsHovered() const override
	{
		return (SSpinBox<NumericType>::IsHovered() & FJolfSlateFocus::bDrawMouseHover) || (UserFocusHoverMask & FJolfSlateFocus::DrawFocusHoverMask);
	}
	//~ End SWidget Interface

private: // Properties

	uint8 UserFocusHoverMask = 0;
};