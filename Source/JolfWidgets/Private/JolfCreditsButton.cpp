// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfCreditsButton.h"
#include "HAL/PlatformProcess.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

// Public Interfaces:

//~ Begin UWidget Interface
void UJolfCreditsButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (ensure(TextBlock))
		TextBlock->SetText(ButtonText);
}
//~ End UWidget Interface

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfCreditsButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(Button))
		Button->OnClicked.AddDynamic(this, &UJolfCreditsButton::OnButtonClicked);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfCreditsButton::OnButtonClicked()
{
	FPlatformProcess::LaunchURL(*LinkUrl, nullptr, nullptr);
}