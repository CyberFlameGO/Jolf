// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "SJolfEditableTextBox.h"
#include "JolfSlateFocus.h"

void SJolfEditableTextBox::Construct(const FArguments& InArgs)
{
	BorderImageNormal = InArgs._BorderImageNormal;
	BorderImageFocused = InArgs._BorderImageFocused;

	SBorder::Construct(SBorder::FArguments()
		.Padding(FMargin(10.0f, 6.0f))
		.BorderImage(this, &SJolfEditableTextBox::GetBorderImage)
		[
			SAssignNew(EditableText, SJolfEditableText)
				.OnTextChanged(InArgs._OnTextChanged)
				.OnTextCommitted(InArgs._OnTextCommitted)
				.Style(InArgs._EditableTextStyle)
		]
	);
}

// Public Interfaces

//~ Begin SWidget Interface
bool SJolfEditableTextBox::SupportsKeyboardFocus() const
{
	return true;
}

bool SJolfEditableTextBox::HasKeyboardFocus() const
{
	// Since keyboard focus is forwarded to our editable text, we will test it instead
	return SBorder::HasKeyboardFocus() || EditableText->HasKeyboardFocus();
}

void SJolfEditableTextBox::OnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	SBorder::OnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

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

FReply SJolfEditableTextBox::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	FReply Reply = FReply::Handled();

	if (InFocusEvent.GetCause() != EFocusCause::Cleared)
	{
		// Forward keyboard focus to our editable text widget
		Reply.SetUserFocus(EditableText.ToSharedRef(), InFocusEvent.GetCause());
	}

	return Reply;
}

FReply SJolfEditableTextBox::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FKey Key = InKeyEvent.GetKey();

	if (Key == EKeys::Escape && EditableText->HasKeyboardFocus())
	{
		// Clear focus
		return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::Cleared);
	}

	return FReply::Unhandled();
}
//~ End SWidget Interface

// Protected Functions:

const FSlateBrush* SJolfEditableTextBox::GetBorderImage() const
{
	if ((IsHovered() & FJolfSlateFocus::bDrawMouseHover) || (UserFocusHoverMask & FJolfSlateFocus::DrawFocusHoverMask))
	{
		return BorderImageFocused;
	}
	else
	{
		return BorderImageNormal;
	}
}