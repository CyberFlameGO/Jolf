// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfCycleButton.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

// Public Functions:

int32 UJolfCycleButton::GetSelectedIndex() const
{
	return SelectedIndex;
}

void UJolfCycleButton::SetSelectedIndex(int32 InIndex)
{
	SelectedIndex = InIndex;
	if (Options.IsValidIndex(SelectedIndex) && ensure(Label))
	{
		Label->SetText(Options[SelectedIndex]);
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfCycleButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(Button))
		Button->OnClicked.AddDynamic(this, &UJolfCycleButton::OnButtonClicked);

	if (Options.IsValidIndex(SelectedIndex) && ensure(Label))
		Label->SetText(Options[SelectedIndex]);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfCycleButton::OnButtonClicked()
{
	SetSelectedIndex((SelectedIndex + 1) % Options.Num());
	OnSelectionChanged.Broadcast(SelectedIndex);
}