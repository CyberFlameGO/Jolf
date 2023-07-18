// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPawnLabel.h"
#include "JolfPawn.h"
#include "Components/TextBlock.h"
#include "JolfPlayerState.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfPawnLabel::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (AJolfPawn* Pawn = WeakPawn.Get())
	{
		if (AJolfPlayerState* PlayerState = Pawn->GetPlayerState<AJolfPlayerState>())
		{
			NameTextBlock->SetText(FText::AsCultureInvariant(PlayerState->GetPlayerName()));
		}
	}
}
//~ End UUserWidget Interface