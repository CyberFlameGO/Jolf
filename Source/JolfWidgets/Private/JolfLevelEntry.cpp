// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfLevelEntry.h"
#include "JolfLevelDefinition.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

UJolfLevelEntry::UJolfLevelEntry(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsFocusable = true;
}

// Public Functions:

void UJolfLevelEntry::SetLevelDef(UJolfLevelDefinition* InLevelDef)
{
	LevelDef = InLevelDef;
	check(LevelDef && InLevelDef);

	if (ensure(ThumbnailImage))
		ThumbnailImage->SetBrushFromTexture(LevelDef->Thumbnail.LoadSynchronous()); // todo

	if (ensure(TitleTextBlock))
		TitleTextBlock->SetText(LevelDef->TitleText);
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfLevelEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (ensure(Button))
		Button->OnClicked.AddDynamic(this, &UJolfLevelEntry::OnButtonClicked);
}

FReply UJolfLevelEntry::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (Button)
	{
		return FReply::Handled().SetUserFocus(Button->TakeWidget(), InFocusEvent.GetCause(), false);
	}
	else
	{
		return FReply::Unhandled();
	}
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfLevelEntry::OnButtonClicked()
{
	OnClicked.Broadcast(LevelDef);
}