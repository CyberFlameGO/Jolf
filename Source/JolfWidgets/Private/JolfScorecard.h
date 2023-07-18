// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Fonts/SlateFontInfo.h"
#include "JolfScorecard.generated.h"

USTRUCT()
struct FJolfScorecardEntry
{
	GENERATED_BODY()

	UPROPERTY()
	class UJolfScorecardPlayerButton* Button;

	UPROPERTY()
	TArray<class UTextBlock*> ScoreTextBlocks;

	UPROPERTY()
	class UTextBlock* TotalScoreTextBlock;

	FDelegateHandle ScoresChangedHandle;
};

UCLASS(Abstract)
class UJolfScorecard : public UUserWidget
{
	GENERATED_BODY()

public:

	UJolfScorecard(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
	FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//~ End UUserWidget Interface

private: // Functions

	int32 IndexOfEntry(const class AJolfPlayerState* InPlayerState) const;
	void OnGameStateSet(class AGameStateBase* InGameState);
	void BindGameState(class AJolfGameState* InGameState);
	void OnPlayerAdded(class AJolfPlayerState* InPlayerState);
	void OnPlayerRemoved(class AJolfPlayerState* InPlayerState);
	void OnScorecardOpened();
	void RebuildRow(class AJolfGameState* GameState, FJolfScorecardEntry& Entry);
	void OnHoleScoresChanged(class AJolfPlayerState* InPlayerState);

	UFUNCTION()
	void OnStrokesClicked();

	UFUNCTION()
	void OnTimeClicked();
	
private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UGridPanel* GridPanel;

	UPROPERTY(Meta = (BindWidget))
	class UButton* StrokesButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* TimeButton;

	UPROPERTY(Transient)
	TArray<FJolfScorecardEntry> PlayerEntries;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UJolfScorecardPlayerButton> PlayerButtonClass;

	UPROPERTY(EditDefaultsOnly)
	FSlateFontInfo HeaderFont;

	UPROPERTY(EditDefaultsOnly)
	FSlateFontInfo ScoreFont;

	UPROPERTY(EditDefaultsOnly)
	FMargin NumberPadding;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EHorizontalAlignment> NumberHAlign;
	
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<EVerticalAlignment> NumberVAlign;

	UPROPERTY(Transient)
	int32 RowCounter;

	UPROPERTY(Transient)
	uint8 bShowTime:1;

	FDelegateHandle GameStateSetEventHandle;
	FDelegateHandle PlayerAddedEventHandle;
	FDelegateHandle PlayerRemovedEventHandle;
};