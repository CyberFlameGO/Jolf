// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfJoinMenu.h"
#include "JolfMenuStackRoot.h"
#include "Components/Button.h"
#include "JolfEditableTextBox.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfJoinMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(TravelButton))
		TravelButton->OnClicked.AddDynamic(this, &UJolfJoinMenu::OnTravelClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfJoinMenu::OnBackClicked);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfJoinMenu::OnTravelClicked()
{
	GEngine->SetClientTravel(GetWorld(), *AddressTextBox->GetText().ToString(), TRAVEL_Absolute);
}

void UJolfJoinMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}