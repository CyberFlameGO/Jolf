// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfDisplayResolutionEntry.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

// Public Functions:

void UJolfDisplayResolutionEntry::SetResolution(const FIntPoint& InResolution)
{
	Resolution = InResolution;

	if (ensure(ResolutionTextBox))
		ResolutionTextBox->SetText(FText::FormatOrdered(INVTEXT("{0} x {1}"), FText::AsNumber(InResolution.X), FText::AsNumber(InResolution.Y)));

	if (ensure(AspectRatioTextBox))
	{
		int32 GCD = FMath::GreatestCommonDivisor(InResolution.X, InResolution.Y);
		AspectRatioTextBox->SetText(FText::FormatOrdered(INVTEXT("{0}:{1}"), FText::AsNumber(InResolution.X / GCD), FText::AsNumber(InResolution.Y / GCD)));
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfDisplayResolutionEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(Button))
		Button->OnClicked.AddDynamic(this, &UJolfDisplayResolutionEntry::OnButtonClicked);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfDisplayResolutionEntry::OnButtonClicked()
{
	OnClicked.Broadcast(Resolution);
}