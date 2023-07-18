// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfChangeLevelButton.h"
#include "JolfLevelDefinition.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

// Public Functions:

void UJolfChangeLevelButton::SetLevelDef(UJolfLevelDefinition* InLevelDef)
{
	LevelDef = InLevelDef;
	
	if (!ensure(ThumbnailImage) || !ensure(TitleTextBlock))
		return;

	if (LevelDef)
	{
		ThumbnailImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		ThumbnailImage->SetBrushFromTexture(LevelDef->Thumbnail.LoadSynchronous()); // todo
		TitleTextBlock->SetText(LevelDef->TitleText);
	}
	else
	{
		ThumbnailImage->SetVisibility(ESlateVisibility::Hidden);
		TitleTextBlock->SetText(NSLOCTEXT("Jolf", "SelectCourseButton", "Select Course"));
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfChangeLevelButton::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(Button))
		Button->OnClicked.AddDynamic(this, &UJolfChangeLevelButton::OnButtonClicked);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfChangeLevelButton::OnButtonClicked()
{
	OnClicked.Broadcast();
}
