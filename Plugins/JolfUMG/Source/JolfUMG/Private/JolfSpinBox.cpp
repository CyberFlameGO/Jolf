// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSpinBox.h"
#include "Widgets/Input/SEditableText.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Text/SlateEditableTextLayout.h"
#include "SJolfSpinBox.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"

static FSpinBoxStyle* DefaultSpinBoxStyle = nullptr;
static FSlateBrush* DefaultBackgroundImageSelected = nullptr;
static FSlateBrush* DefaultCaretImage = nullptr;

UJolfSpinBox::UJolfSpinBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if (DefaultSpinBoxStyle == nullptr)
	{
		// Copy UMG button style.
		DefaultSpinBoxStyle = new FSpinBoxStyle(WidgetStyle);
		DefaultSpinBoxStyle->TextPadding = FMargin(10.f, 6.f);
		DefaultSpinBoxStyle->ForegroundColor = FLinearColor::White;
		DefaultSpinBoxStyle->ArrowsImage.DrawAs = ESlateBrushDrawType::NoDrawType;
		
		// Unfortunately there is not a good way to style the rounded fill brushes.
		DefaultSpinBoxStyle->ActiveFillBrush.DrawAs = ESlateBrushDrawType::NoDrawType;
		DefaultSpinBoxStyle->InactiveFillBrush.DrawAs = ESlateBrushDrawType::NoDrawType;

		UObject* NormalMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/EditableTextBox/MI_EditableTextBox_Normal"));
		DefaultSpinBoxStyle->BackgroundBrush.ImageSize = FVector2D(64.f, 64.f);
		DefaultSpinBoxStyle->BackgroundBrush.SetResourceObject(NormalMaterial);

		UObject* FocusedMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/EditableTextBox/MI_EditableTextBox_Focused"));
		DefaultSpinBoxStyle->HoveredBackgroundBrush.ImageSize = FVector2D(64.f, 64.f);
		DefaultSpinBoxStyle->HoveredBackgroundBrush.SetResourceObject(FocusedMaterial);
	}

	if (DefaultBackgroundImageSelected == nullptr)
	{
		UObject* Material = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/EditableTextBox/MI_EditableText_BackgroundSelected"));
		DefaultBackgroundImageSelected = new FSlateBrush();
		DefaultBackgroundImageSelected->SetResourceObject(Material);
	}

	if (DefaultCaretImage == nullptr)
	{
		UObject* Material = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/EditableTextBox/MI_EditableText_Caret"));
		DefaultCaretImage = new FSlateBrush();
		DefaultCaretImage->SetResourceObject(Material);
	}

	WidgetStyle = *DefaultSpinBoxStyle;
	BackgroundImageSelected = *DefaultBackgroundImageSelected;
	CaretImage = *DefaultCaretImage;

	if (!IsRunningDedicatedServer())
	{
		static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());
		Font = FSlateFontInfo(RobotoFontObj.Object, 24, FName("Bold"));
	}

	static ConstructorHelpers::FObjectFinder<UObject> FontMaterial(TEXT("/Game/Materials/UserInterface/MI_DarkFont"));
	Font.FontMaterial = FontMaterial.Object;

	ForegroundColor = FLinearColor::White;
	UnderlyingUnits = EUnit::Unspecified;
}

//~ Begin UWidget Interface
void UJolfSpinBox::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	/* 2023-07-17: commenting this out so that it can be compiled with Rocket build of engine.

	// Specify background color so EditableText selection brush is used instead.
	FTextBlockStyle NewTextStyle = MySpinBox->EditableText->EditableTextLayout->GetTextStyle();
	NewTextStyle.SelectedBackgroundColor = FLinearColor::White;
	NewTextStyle.HighlightShape = BackgroundImageSelected;
	MySpinBox->EditableText->EditableTextLayout->SetTextStyle(NewTextStyle);
	MySpinBox->EditableText->EditableTextLayout->SetCursorBrush(&CaretImage);

	*/
}
//~ End UWidget Interface

// Protected Interfaces:

//~ Begin UWidget Interface
TSharedRef<SWidget> UJolfSpinBox::RebuildWidget()
{
	MySpinBox = SNew(SJolfSpinBox<float>)
		.Style(&WidgetStyle)
		.Font(Font)
		.ClearKeyboardFocusOnCommit(ClearKeyboardFocusOnCommit)
		.SelectAllTextOnCommit(SelectAllTextOnCommit)
		.Justification(Justification)
		.OnValueChanged(BIND_UOBJECT_DELEGATE(FOnFloatValueChanged, HandleOnValueChanged))
		.OnValueCommitted(BIND_UOBJECT_DELEGATE(FOnFloatValueCommitted, HandleOnValueCommitted))
		.OnBeginSliderMovement(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnBeginSliderMovement))
		.OnEndSliderMovement(BIND_UOBJECT_DELEGATE(FOnFloatValueChanged, HandleOnEndSliderMovement))
		.TypeInterface(MakeShared<TNumericUnitTypeInterface<float>>(UnderlyingUnits))
		;

	return MySpinBox.ToSharedRef();
}
//~ End UWidget Interface