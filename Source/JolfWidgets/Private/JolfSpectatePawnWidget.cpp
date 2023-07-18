// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSpectatePawnWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/TextBlock.h"
#include "JolfPlayerState.h"
#include "JolfPawn.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfSpectatePawnWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	APlayerController* PC = GetOwningPlayer();
	AActor* ViewTarget = PC->GetViewTarget();
	if (AJolfPawn* Pawn = Cast<AJolfPawn>(ViewTarget))
	{
		if (AJolfPlayerState* PlayerState = Pawn->GetPlayerState<AJolfPlayerState>())
		{	
			const FText PlayerNameText = FText::AsCultureInvariant(PlayerState->GetPlayerName());
			const FText MessageText = FText::FormatOrdered(INVTEXT("Spectating {0}"), PlayerNameText);
			ViewTargetTextBlock->SetText(MessageText);
		}
	}
}
//~ End UUserWidget Interface