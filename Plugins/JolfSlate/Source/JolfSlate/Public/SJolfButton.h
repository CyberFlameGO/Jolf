// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Widgets/Input/SButton.h"

class JOLFSLATE_API SJolfButton : public SButton
{
public: // Interfaces

	//~ Begin SWidget Interface
	void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	bool IsHovered() const override;
	//~ End SWidget Interface

private: // Properties

	uint8 UserFocusHoverMask = 0;
};