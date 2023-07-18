// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfEditableTextBox.h"
#include "SJolfEditableTextBox.h"
#include "Fonts/SlateFontInfo.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Font.h"

static FEditableTextStyle* DefaultEditableTextStyle = nullptr;
static FSlateBrush* DefaultEditableTextBorderImageNormal = nullptr;
static FSlateBrush* DefaultEditableTextBorderImageFocused = nullptr;

UJolfEditableTextBox::UJolfEditableTextBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if (DefaultEditableTextStyle == nullptr)
	{
		DefaultEditableTextStyle = new FEditableTextStyle();
		DefaultEditableTextStyle->ColorAndOpacity = FLinearColor::White;

		if (!IsRunningDedicatedServer())
		{
			static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
			DefaultEditableTextStyle->Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
		}

		UObject* CaretMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/EditableTextBox/MI_EditableText_Caret"));
		DefaultEditableTextStyle->CaretImage.ImageSize = FVector2D(64.f, 64.f);
		DefaultEditableTextStyle->CaretImage.SetResourceObject(CaretMaterial);

		UObject* BackgroundSelectedMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/EditableTextBox/MI_EditableText_BackgroundSelected"));
		DefaultEditableTextStyle->BackgroundImageSelected.ImageSize = FVector2D(64.f, 64.f);
		DefaultEditableTextStyle->BackgroundImageSelected.SetResourceObject(BackgroundSelectedMaterial);

		UObject* FontMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/MI_DarkFont"));
		DefaultEditableTextStyle->Font.FontMaterial = FontMaterial;
	}

	if (DefaultEditableTextBorderImageNormal == nullptr)
	{
		UObject* Material = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/EditableTextBox/MI_EditableTextBox_Normal"));
		DefaultEditableTextBorderImageNormal = new FSlateBrush();
		DefaultEditableTextBorderImageNormal->ImageSize = FVector2D(64.f, 64.f);
		DefaultEditableTextBorderImageNormal->DrawAs = ESlateBrushDrawType::Box;
		DefaultEditableTextBorderImageNormal->Margin = FMargin(0.25f);
		DefaultEditableTextBorderImageNormal->SetResourceObject(Material);
	}

	if (DefaultEditableTextBorderImageFocused == nullptr)
	{
		UObject* Material = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/EditableTextBox/MI_EditableTextBox_Focused"));
		DefaultEditableTextBorderImageFocused = new FSlateBrush();
		DefaultEditableTextBorderImageFocused->ImageSize = FVector2D(64.f, 64.f);
		DefaultEditableTextBorderImageFocused->DrawAs = ESlateBrushDrawType::Box;
		DefaultEditableTextBorderImageFocused->Margin = FMargin(0.25f);
		DefaultEditableTextBorderImageFocused->SetResourceObject(Material);
	}

	EditableTextStyle = *DefaultEditableTextStyle;
	BorderImageNormal = *DefaultEditableTextBorderImageNormal;
	BorderImageFocused = *DefaultEditableTextBorderImageFocused;
}

// Public Functions:

FText UJolfEditableTextBox::GetText() const
{
	if (MyEditableTextBox.IsValid())
	{
		return MyEditableTextBox->EditableText->GetText();
	}

	return Text;
}

void UJolfEditableTextBox::SetText(FText InText)
{
	Text = InText;
	if (MyEditableTextBox.IsValid())
	{
		MyEditableTextBox->EditableText->SetText(Text);
	}
}

// Public Interfaces:

//~ Begin UVisual Interface
void UJolfEditableTextBox::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyEditableTextBox.Reset();
}
//~ End UVisual Interface

// Protected Interfaces:

//~ Begin UWidget Interface
TSharedRef<SWidget> UJolfEditableTextBox::RebuildWidget()
{
	TSharedRef<SJolfEditableTextBox> MyEditableTextBlockRef = SAssignNew(MyEditableTextBox, SJolfEditableTextBox)
		.OnTextChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnTextChanged))
		.OnTextCommitted(BIND_UOBJECT_DELEGATE(FOnTextCommitted, HandleOnTextCommitted))
		.BorderImageNormal(&BorderImageNormal)
		.BorderImageFocused(&BorderImageFocused)
		.EditableTextStyle(&EditableTextStyle);
	return MyEditableTextBlockRef;
}
//~ End UWidget Interface

// Private Functions:

void UJolfEditableTextBox::HandleOnTextChanged(const FText& InText)
{
	Text = InText;
	OnTextChanged.Broadcast(InText);
}

void UJolfEditableTextBox::HandleOnTextCommitted(const FText& InText, ETextCommit::Type CommitMethod)
{
	Text = InText;
	OnTextCommitted.Broadcast(InText, CommitMethod);
}