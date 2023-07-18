// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfCreditsMenu.h"
#include "Components/Button.h"
#include "JolfMenuStackRoot.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfCreditsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfCreditsMenu::OnBackClicked);
}
//~ End UUserWidget Interface

void UJolfCreditsMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}