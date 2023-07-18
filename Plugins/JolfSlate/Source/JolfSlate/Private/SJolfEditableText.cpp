// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "SJolfEditableText.h"
#include "Widgets/Text/SlateEditableTextLayout.h"

void SJolfEditableText::Construct(const FArguments& InArgs)
{
	SEditableText::Construct(InArgs);

	// Specify background color so EditableText selection brush is used instead.
	FTextBlockStyle NewTextStyle = EditableTextLayout->GetTextStyle();
	NewTextStyle.SelectedBackgroundColor = FLinearColor::White;
	EditableTextLayout->SetTextStyle(NewTextStyle);
}