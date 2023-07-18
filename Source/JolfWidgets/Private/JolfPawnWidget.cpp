// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPawnWidget.h"
#include "JolfPawn.h"
#include "JolfPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "JolfHole.h"
#include "JolfPawnLabel.h"
#include "Engine/World.h"
#include "JolfGameState.h"
#include "JolfProjectionPanel.h"
#include "JolfProjectionPanelSlot.h"
#include "Components/StaticMeshComponent.h"
#include "JolfPlayerController.h"
#include "JolfViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "JolfLocalPlayer.h"
#include "Components/ScrollBox.h"
#include "JolfTextChatEntry.h"

//~ Begin UUserWidget Interface
void UJolfPawnWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ensure(ArrowImage))
		ArrowMaterial = ArrowImage->GetDynamicMaterial();
}

void UJolfPawnWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AJolfPlayerState* OwningPlayerState = GetOwningPlayerState<AJolfPlayerState>();
	UJolfLocalPlayer* OwningLocalPlayer = GetOwningLocalPlayer<UJolfLocalPlayer>();
	if (OwningPlayerState == nullptr || OwningLocalPlayer == nullptr)
		return;
	const AJolfHole* Hole = OwningPlayerState->GetCurrentHoleActor();

	if (AJolfPawn* Pawn = GetOwningPlayerPawn<AJolfPawn>())
	{
		FLinearColor OuterColor = FMath::Lerp(FLinearColor::Red, FLinearColor::Green, Pawn->GetInputStrength());
		ESlateVisibility StrengthVisibility = Pawn->IsLaunching() ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed;

		ArrowMaterial->SetScalarParameterValue(TEXT("Power"), Pawn->GetInputStrength());
		ArrowImage->SetColorAndOpacity(OuterColor);
		ArrowImage->SetVisibility(StrengthVisibility);
		ArrowImage->SetRenderTransformAngle(-Pawn->GetInputPitch());

		if (OwningLocalPlayer->GetPlayerSettings()->bShowStrokePowerPercentage)
		{
			StrengthTextBlock->SetVisibility(StrengthVisibility);
			if (Pawn->GetInputPitch() == 0.f)
			{
				StrengthTextBlock->SetText(FText::AsPercent(Pawn->GetInputStrength()));
			}
			else
			{
				FNumberFormattingOptions FormatOptions;
				FormatOptions.MinimumFractionalDigits = 1;
				FormatOptions.MaximumFractionalDigits = 1;
				FText InputText = FText::FormatOrdered(INVTEXT("{0} {1}°"), FText::AsPercent(Pawn->GetInputStrength()), FText::AsNumber(Pawn->GetInputPitch(), &FormatOptions));
				StrengthTextBlock->SetText(InputText);
			}
		}
		else
		{
			StrengthTextBlock->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (Hole)
	{
		HoleTextBlock->SetText(FText::FormatOrdered(INVTEXT("Hole {0}"), FText::AsNumber(Hole->HoleIndex + 1)));
		ParTextBlock->SetText(FText::FormatOrdered(INVTEXT("Par {0}"), FText::AsNumber(Hole->Par)));
	}
	else
	{
		HoleTextBlock->SetText(INVTEXT("Hole N/A"));
		ParTextBlock->SetText(INVTEXT("Par N/A"));
	}

	StrokeTextBlock->SetText(FText::FormatOrdered(INVTEXT("Strokes {0}"), FText::AsNumber(OwningPlayerState->StrokeIndex)));

	PawnLabels.RemoveAllSwap([](UJolfPawnLabel* Label)
	{
		if (Label->WeakPawn.IsValid())
		{
			return false;
		}
		else
		{
			Label->RemoveFromParent();
			return true;
		}
	});

	const AJolfPlayerController* PlayerController = GetOwningPlayer<AJolfPlayerController>();

	bool bHotseat = false;
	if (const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer())
	{
		if (UJolfViewportClient* ViewportClient = Cast<UJolfViewportClient>(LocalPlayer->ViewportClient))
		{
			if (ViewportClient->GetHotseatIndex() != INDEX_NONE)
			{
				bHotseat = true;
			}
		}
	}

	if (bHotseat)
	{
		OwnerNameTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
		OwnerNameTextBlock->SetText(FText::AsCultureInvariant(OwningPlayerState->GetPlayerName()));
	}
	else
	{
		OwnerNameTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (PlayerController->bWantsOverlay)
	{
		ProjectionCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (UJolfProjectionPanelSlot* MarkerSlot = Cast<UJolfProjectionPanelSlot>(HoleMarkerTextBlock->Slot))
		{
			if (Hole)
			{
				MarkerSlot->WorldLocation = Hole->GetActorLocation();
				HoleMarkerTextBlock->SetText(HoleTextBlock->GetText());
			}
		}

		if (AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
		{
			for (APlayerState* PlayerStateBase : GameState->PlayerArray)
			{
				if (PlayerStateBase == OwningPlayerState)
				{
					// Skip 3D label for self.
					continue;
				}

				AJolfPlayerState* PlayerState = Cast<AJolfPlayerState>(PlayerStateBase);
				if (PlayerState == nullptr)
					continue;

				AJolfPawn* Pawn = PlayerState->GetPawn<AJolfPawn>();
				if (Pawn == nullptr)
					continue;

				UJolfPawnLabel* Label;
				UJolfPawnLabel** LabelPtr = PawnLabels.FindByPredicate([Pawn](UJolfPawnLabel* TestLabel)
				{
					return TestLabel->WeakPawn.Get() == Pawn;
				});
				if (LabelPtr)
				{
					Label = *LabelPtr;
				}
				else
				{
					Label = CreateWidget<UJolfPawnLabel>(this, LabelClass);
					Label->WeakPawn = Pawn;
					ProjectionCanvas->AddChild(Label);
					check(Label);
					PawnLabels.Add(Label);
				}

				if (UJolfProjectionPanelSlot* LabelSlot = Cast<UJolfProjectionPanelSlot>(Label->Slot))
				{
					LabelSlot->WorldLocation = Pawn->GetMeshComponent()->GetComponentLocation();
				}
			}
		}

		ProjectionCanvas->TickLayout();
	}
	else
	{
		ProjectionCanvas->SetVisibility(ESlateVisibility::Collapsed);
	}
}
//~ End UUserWidget Interface
