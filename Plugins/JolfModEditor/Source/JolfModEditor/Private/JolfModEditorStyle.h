// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Styling/SlateStyle.h"

class FJolfModEditorStyle
{
public: // Functions

	static void Initialize();
	static void Shutdown();
	static const ISlateStyle& Get();

private: // Functions

	static TSharedRef<FSlateStyleSet> Create();

private: // Properties

	static TSharedPtr<FSlateStyleSet> StyleInstance;
};