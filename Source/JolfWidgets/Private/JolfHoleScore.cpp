// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfHoleScore.h"
#include "Components/TextBlock.h"
#include "JolfPawn.h"
#include "JolfHole.h"
#include "JolfWidgetsUtils.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfHoleScore::NativeConstruct()
{
	Super::NativeConstruct();
	
	AJolfPawn* Pawn = GetOwningPlayerPawn<AJolfPawn>();
	int32 NumStrokes = Pawn->GetScore().NumStrokes;
	int32 Par = Pawn->GetOverlappingHole()->Par;

	FText TitleText;
	FText DescriptionText;
	
	if (NumStrokes == 1 && Par > 1)
	{
		TitleText = INVTEXT("HOLE IN ONE");
		DescriptionText = FText::GetEmpty();
	}
	else
	{
		if (NumStrokes < Par)
		{
			DescriptionText = FText::FormatOrdered(INVTEXT("{0} below par"), Par - NumStrokes);
		}
		else if (NumStrokes > Par)
		{
			DescriptionText = FText::FormatOrdered(INVTEXT("{0} above par"), NumStrokes - Par);
		}
		else
		{
			DescriptionText = FText::GetEmpty();
		}

		switch (NumStrokes - Par)
		{
			case -3:
				TitleText = INVTEXT("ALBATROSS");
				break;

			case -2:
				TitleText = INVTEXT("EAGLE");
				break;

			case -1:
				TitleText = INVTEXT("BIRB");
				break;

			case 0:
				TitleText = INVTEXT("PAR");
				break;

			case 1:
				TitleText = INVTEXT("BOGEY");
				break;

			case 2:
				TitleText = INVTEXT("BOOGERS");
				break;

			default:
				TitleText = DescriptionText;
				DescriptionText = FText::GetEmpty();
				break;
		}
	}

	TitleTextBlock->SetText(TitleText);
	DescriptionTextBlock->SetText(DescriptionText);
	TimeTextBlock->SetText(JolfWidgetsUtils::SecondsAsText(Pawn->GetScore().Time));

	PlayAnimation(PopupAnim);
}
//~ End UUserWidget Interface