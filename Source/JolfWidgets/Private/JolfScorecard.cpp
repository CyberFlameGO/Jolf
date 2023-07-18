// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfScorecard.h"
#include "Components/GridPanel.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "JolfPlayerController.h"
#include "JolfPlayerState.h"
#include "JolfHole.h"
#include "JolfGameState.h"
#include "JolfScorecardPlayerButton.h"
#include "Components/GridSlot.h"

UJolfScorecard::UJolfScorecard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsFocusable = true;
	RowCounter = 2;
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfScorecard::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(StrokesButton))
		StrokesButton->OnClicked.AddDynamic(this, &UJolfScorecard::OnStrokesClicked);

	if (ensure(TimeButton))
		TimeButton->OnClicked.AddDynamic(this, &UJolfScorecard::OnTimeClicked);
}

void UJolfScorecard::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->OnScorecardOpened.AddUObject(this, &UJolfScorecard::OnScorecardOpened);
	}

	UWorld* World = GetWorld();
	check(World);
	if (AJolfGameState* GameState = World->GetGameState<AJolfGameState>())
	{
		BindGameState(GameState);
	}
	else
	{
		GameStateSetEventHandle = World->GameStateSetEvent.AddUObject(this, &UJolfScorecard::OnGameStateSet);
	}
}

void UJolfScorecard::NativeDestruct()
{
	UWorld* World = GetWorld();
	check(World);
	if (GameStateSetEventHandle.IsValid())
	{
		World->GameStateSetEvent.Remove(GameStateSetEventHandle);
		GameStateSetEventHandle.Reset();
	}

	if (AJolfGameState* GameState = World->GetGameState<AJolfGameState>())
	{
		GameState->OnPlayerAdded.Remove(PlayerAddedEventHandle);
		PlayerAddedEventHandle.Reset();
		GameState->OnPlayerRemoved.Remove(PlayerRemovedEventHandle);
		PlayerRemovedEventHandle.Reset();
	}

	Super::NativeDestruct();
}

FReply UJolfScorecard::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (AJolfPlayerState* OwningPlayerState = GetOwningPlayerState<AJolfPlayerState>())
	{
		int32 EntryIndex = IndexOfEntry(OwningPlayerState);
		if (EntryIndex != INDEX_NONE)
		{
			FJolfScorecardEntry& Entry = PlayerEntries[EntryIndex];
			return FReply::Handled().SetUserFocus(Entry.Button->TakeWidget(), InFocusEvent.GetCause(), false);
		}
	}

	return FReply::Unhandled();
}

FReply UJolfScorecard::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const EUINavigationAction NavAction = FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent);
	if (NavAction == EUINavigationAction::Back)
	{
		if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
		{
			PC->CloseScorecard();
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
//~ End UUserWidget Interface

int32 UJolfScorecard::IndexOfEntry(const class AJolfPlayerState* InPlayerState) const
{
	return PlayerEntries.IndexOfByPredicate([InPlayerState](const FJolfScorecardEntry& Entry) -> bool
	{
		return Entry.Button->GetPlayerState() == InPlayerState;
	});
}

void UJolfScorecard::OnGameStateSet(AGameStateBase* InGameState)
{
	AJolfGameState* TypedGameState = Cast<AJolfGameState>(InGameState);
	if (ensure(TypedGameState))
	{
		GetWorld()->GameStateSetEvent.Remove(GameStateSetEventHandle);
		GameStateSetEventHandle.Reset();
		BindGameState(TypedGameState);
	}
}

void UJolfScorecard::BindGameState(AJolfGameState* InGameState)
{
	PlayerAddedEventHandle = InGameState->OnPlayerAdded.AddUObject(this, &UJolfScorecard::OnPlayerAdded);
	PlayerRemovedEventHandle = InGameState->OnPlayerRemoved.AddUObject(this, &UJolfScorecard::OnPlayerRemoved);

	for (APlayerState* Player : InGameState->PlayerArray)
	{
		if (AJolfPlayerState* TypedPlayer = Cast<AJolfPlayerState>(Player))
		{
			OnPlayerAdded(TypedPlayer);
		}
	}

	int32 TotalPar = 0;

	for (AJolfHole* Hole : InGameState->Holes)
	{
		UTextBlock* HoleNumberTextBlock = NewObject<UTextBlock>(GridPanel);
		HoleNumberTextBlock->SetText(FText::AsNumber(Hole->HoleIndex + 1));
		HoleNumberTextBlock->SetFont(HeaderFont);
		UGridSlot* HoleNumberSlot = GridPanel->AddChildToGrid(HoleNumberTextBlock, 0, Hole->HoleIndex + 1);
		HoleNumberSlot->SetPadding(NumberPadding);
		HoleNumberSlot->SetHorizontalAlignment(NumberHAlign);
		HoleNumberSlot->SetVerticalAlignment(NumberVAlign);

		UTextBlock* ParTextBlock = NewObject<UTextBlock>(GridPanel);
		ParTextBlock->SetText(FText::AsNumber(Hole->Par));
		ParTextBlock->SetFont(HeaderFont);
		UGridSlot* ParNumberSlot = GridPanel->AddChildToGrid(ParTextBlock, 1, Hole->HoleIndex + 1);
		ParNumberSlot->SetPadding(NumberPadding);
		ParNumberSlot->SetHorizontalAlignment(NumberHAlign);
		ParNumberSlot->SetVerticalAlignment(NumberVAlign);
	
		TotalPar += Hole->Par;
	}

	{
		UTextBlock* ParTextBlock = NewObject<UTextBlock>(GridPanel);
		ParTextBlock->SetText(FText::AsNumber(TotalPar));
		ParTextBlock->SetFont(HeaderFont);
		UGridSlot* ParNumberSlot = GridPanel->AddChildToGrid(ParTextBlock, 1, InGameState->Holes.Num() + 1);
		ParNumberSlot->SetPadding(NumberPadding);
		ParNumberSlot->SetHorizontalAlignment(NumberHAlign);
		ParNumberSlot->SetVerticalAlignment(NumberVAlign);
	}
}

void UJolfScorecard::OnPlayerAdded(AJolfPlayerState* InPlayerState)
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	if (!ensure(GameState))
		return;

	UJolfScorecardPlayerButton* Button = CreateWidget<UJolfScorecardPlayerButton>(this, PlayerButtonClass);
	if (ensure(Button))
	{
		Button->SetPlayerState(InPlayerState);
		UGridSlot* ButtonSlot = GridPanel->AddChildToGrid(Button, RowCounter, 0);
		ButtonSlot->SetColumnSpan(1 + GameState->Holes.Num() + 1);

		FJolfScorecardEntry& Entry = PlayerEntries.AddDefaulted_GetRef();
		Entry.Button = Button;
		Entry.ScoreTextBlocks.SetNumUninitialized(GameState->Holes.Num());
		for (AJolfHole* Hole : GameState->Holes)
		{
			UTextBlock* ScoreTextBlock = NewObject<UTextBlock>(GridPanel);
			ScoreTextBlock->SetFont(ScoreFont);
			UGridSlot* ScoreSlot = GridPanel->AddChildToGrid(ScoreTextBlock, RowCounter, Hole->HoleIndex + 1);
			ScoreSlot->SetPadding(NumberPadding);
			ScoreSlot->SetHorizontalAlignment(NumberHAlign);
			ScoreSlot->SetVerticalAlignment(NumberVAlign);
			Entry.ScoreTextBlocks[Hole->HoleIndex] = ScoreTextBlock;
		}

		{
			Entry.TotalScoreTextBlock = NewObject<UTextBlock>(GridPanel);
			Entry.TotalScoreTextBlock->SetFont(ScoreFont);
			UGridSlot* ScoreSlot = GridPanel->AddChildToGrid(Entry.TotalScoreTextBlock, RowCounter, GameState->Holes.Num() + 1);
			ScoreSlot->SetPadding(NumberPadding);
			ScoreSlot->SetHorizontalAlignment(NumberHAlign);
			ScoreSlot->SetVerticalAlignment(NumberVAlign);
		}

		Entry.ScoresChangedHandle = InPlayerState->OnHoleScoresChanged.AddUObject(this, &UJolfScorecard::OnHoleScoresChanged);
	}
	++RowCounter;
}

void UJolfScorecard::OnPlayerRemoved(AJolfPlayerState* InPlayerState)
{
	int32 EntryIndex = IndexOfEntry(InPlayerState);
	FJolfScorecardEntry& Entry = PlayerEntries[EntryIndex];
	GridPanel->RemoveChild(Entry.Button);
	for (UTextBlock* ScoreTextBlock : Entry.ScoreTextBlocks)
	{
		GridPanel->RemoveChild(ScoreTextBlock);
	}
	InPlayerState->OnHoleScoresChanged.Remove(Entry.ScoresChangedHandle);
	PlayerEntries.RemoveAtSwap(EntryIndex);
}

void UJolfScorecard::OnScorecardOpened()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	if (GameState == nullptr)
		return;

	for (FJolfScorecardEntry& Entry : PlayerEntries)
	{
		RebuildRow(GameState, Entry);
	}
}

void UJolfScorecard::RebuildRow(AJolfGameState* GameState, FJolfScorecardEntry& Entry)
{
	AJolfPlayerState* JolfPS = Entry.Button->GetPlayerState();
	int32 TotalPar = 0;
	for (AJolfHole* Hole : GameState->Holes)
	{
		UTextBlock* ScoreTextBlock = Entry.ScoreTextBlocks[Hole->HoleIndex];

		const FJolfPlayerStateScore* HoleScore = JolfPS->GetHoleScores().FindByPredicate([Hole](FJolfPlayerStateScore& Entry)
		{
			return Entry.HoleIndex == Hole->HoleIndex;
		});
		if (HoleScore)
		{
			TotalPar += HoleScore->Strokes;
			ScoreTextBlock->SetText(bShowTime ? JolfWidgetsUtils::SecondsAsText(HoleScore->Time) : FText::AsNumber(HoleScore->Strokes));
		}
		else
		{
			ScoreTextBlock->SetText(INVTEXT("-"));
		}
	}

	if (TotalPar > 0)
	{
		Entry.TotalScoreTextBlock->SetText(FText::AsNumber(TotalPar));
	}
	else
	{
		Entry.TotalScoreTextBlock->SetText(INVTEXT("-"));
	}
}

void UJolfScorecard::OnHoleScoresChanged(AJolfPlayerState* InPlayerState)
{
	if (!IsVisible())
		return;

	if (AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
	{
		int32 EntryIndex = IndexOfEntry(InPlayerState);
		FJolfScorecardEntry& Entry = PlayerEntries[EntryIndex];
		RebuildRow(GameState, Entry);
	}
}

void UJolfScorecard::OnStrokesClicked()
{
	bShowTime = false;
	OnScorecardOpened();
}

void UJolfScorecard::OnTimeClicked()
{
	bShowTime = true;
	OnScorecardOpened();
}