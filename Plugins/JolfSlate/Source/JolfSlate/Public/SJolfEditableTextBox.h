// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Input/Reply.h"
#include "Layout/Margin.h"
#include "Misc/Attribute.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateColor.h"
#include "Styling/SlateTypes.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SJolfEditableText.h"
#include "Widgets/Layout/SBorder.h"
#include "Framework/SlateDelegates.h"

/**
 * Based on SEditableTextBox.h
 * Exists to customize SEditableText styling properties that can only be set during construction.
 */
class JOLFSLATE_API SJolfEditableTextBox : public SBorder
{
public:

	SLATE_BEGIN_ARGS(SJolfEditableTextBox) {}
	
	/** Called whenever the text is changed programmatically or interactively by the user */
	SLATE_EVENT( FOnTextChanged, OnTextChanged )

	/** Called whenever the text is committed.  This happens when the user presses enter or the text box loses focus. */
	SLATE_EVENT( FOnTextCommitted, OnTextCommitted )

	SLATE_ARGUMENT(const FSlateBrush*, BorderImageNormal)
	SLATE_ARGUMENT(const FSlateBrush*, BorderImageFocused)
	
	SLATE_STYLE_ARGUMENT(FEditableTextStyle, EditableTextStyle)

	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);

public: // Interfaces

	//~ Begin SWidget Interface
	bool SupportsKeyboardFocus() const override;
	bool HasKeyboardFocus() const override;
	void OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;
	FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	//~ End SWidget Interface

public: // Properties

	TSharedPtr<SJolfEditableText> EditableText;
	const FSlateBrush* BorderImageNormal;
	const FSlateBrush* BorderImageFocused;

protected: // Functions

	const FSlateBrush* GetBorderImage() const;

private: // Properties

	uint8 UserFocusHoverMask = 0;
};