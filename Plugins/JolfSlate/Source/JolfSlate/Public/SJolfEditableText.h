// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Widgets/Input/SEditableText.h"

/** Specifies selection background color so selection brush material can be any color. */
class JOLFSLATE_API SJolfEditableText : public SEditableText
{
public:

	void Construct(const FArguments& InArgs);
};