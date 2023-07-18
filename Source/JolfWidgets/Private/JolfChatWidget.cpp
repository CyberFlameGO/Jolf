// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfChatWidget.h"
#include "JolfPlayerController.h"
#include "JolfLocalPlayer.h"
#include "Components/ScrollBox.h"
#include "JolfTextChatEntry.h"
#include "Components/TextBlock.h"
#include "JolfPlayerState.h"
#include "JolfEditableTextBox.h"

//~ Begin UUserWidget Interface
void UJolfChatWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (ensure(TextChatField))
		TextChatField->OnTextCommitted.AddDynamic(this, &UJolfChatWidget::OnTextChatCommitted);
}

void UJolfChatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->OnTextChatRequested.AddUObject(this, &UJolfChatWidget::OnTextChatRequested);
		PC->OnTextChatReceived.AddUObject(this, &UJolfChatWidget::OnTextChatReceived);
	}
}

void UJolfChatWidget::NativeDestruct()
{
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->OnTextChatRequested.RemoveAll(this);
		PC->OnTextChatReceived.RemoveAll(this);
	}
	
	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfChatWidget::OnTextChatRequested()
{
	TextChatField->SetVisibility(ESlateVisibility::Visible);

	if (UJolfLocalPlayer* LP = GetOwningLocalPlayer<UJolfLocalPlayer>())
	{
		LP->SetSlateUserFocus(TextChatField);
	}
}

void UJolfChatWidget::OnTextChatCommitted(const FText& InText, ETextCommit::Type InCommitMethod)
{
	if (InCommitMethod == ETextCommit::OnEnter && !InText.IsEmptyOrWhitespace())
	{
		if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
		{
			PC->ServerTextChat(InText);
		}
	}

	TextChatField->SetText(FText::GetEmpty());
	TextChatField->SetVisibility(ESlateVisibility::Hidden);

	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->OnTextChatFinished();
	}
}

void UJolfChatWidget::OnTextChatReceived(AJolfPlayerState* InSpeaker, const FText& InText)
{
	UJolfTextChatEntry* Entry = CreateWidget<UJolfTextChatEntry>(this, TextChatEntryClass);
	if (ensure(Entry))
	{
		Entry->MessageTextBlock->SetText(FText::FormatOrdered(INVTEXT("{0}: {1}"), FText::AsCultureInvariant(InSpeaker->GetPlayerName()), InText));
		TextChatHistory->AddChild(Entry);
		TextChatHistory->ScrollToEnd();
	}
}