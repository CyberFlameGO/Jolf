// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Framework/Application/NavigationConfig.h"

class FJolfSlateNavigationConfig : public FNavigationConfig
{
public: // Interfaces

	//~ Begin FNavigationConfig Interface
	void OnNavigationChangedFocus(TSharedPtr<SWidget> OldWidget, TSharedPtr<SWidget> NewWidget, FFocusEvent FocusEvent) override;
	//~ End FNavigationConfig Interface
};