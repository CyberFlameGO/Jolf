// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfCheckBox.h"
#include "SJolfCheckBox.h"

static FCheckBoxStyle* DefaultCheckBoxStyle = nullptr;

UJolfCheckBox::UJolfCheckBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if (DefaultCheckBoxStyle == nullptr)
	{
		// Copy UMG check box style.
		DefaultCheckBoxStyle = new FCheckBoxStyle(WidgetStyle);
		DefaultCheckBoxStyle->ForegroundColor = FLinearColor::White;
		DefaultCheckBoxStyle->BorderBackgroundColor = FLinearColor::White;

		UObject* UncheckedNormalMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/CheckBox/MI_CheckBox_Unchecked_Normal"));
		DefaultCheckBoxStyle->UncheckedImage.ImageSize = FVector2D(48.f, 48.f);
		DefaultCheckBoxStyle->UncheckedImage.TintColor = FLinearColor::White;
		DefaultCheckBoxStyle->UncheckedImage.SetResourceObject(UncheckedNormalMaterial);

		UObject* UncheckedHoveredMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/CheckBox/MI_CheckBox_Unchecked_Hovered"));
		DefaultCheckBoxStyle->UncheckedHoveredImage.ImageSize = FVector2D(48.f, 48.f);
		DefaultCheckBoxStyle->UncheckedHoveredImage.TintColor = FLinearColor::White;
		DefaultCheckBoxStyle->UncheckedHoveredImage.SetResourceObject(UncheckedHoveredMaterial);

		UObject* UncheckedPressedMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/CheckBox/MI_CheckBox_Unchecked_Pressed"));
		DefaultCheckBoxStyle->UncheckedPressedImage.ImageSize = FVector2D(48.f, 48.f);
		DefaultCheckBoxStyle->UncheckedPressedImage.TintColor = FLinearColor::White;
		DefaultCheckBoxStyle->UncheckedPressedImage.SetResourceObject(UncheckedPressedMaterial);

		UObject* CheckedNormalMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/CheckBox/MI_CheckBox_Checked_Normal"));
		DefaultCheckBoxStyle->CheckedImage.ImageSize = FVector2D(48.f, 48.f);
		DefaultCheckBoxStyle->CheckedImage.TintColor = FLinearColor::White;
		DefaultCheckBoxStyle->CheckedImage.SetResourceObject(CheckedNormalMaterial);

		UObject* CheckedHoveredMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/CheckBox/MI_CheckBox_Checked_Hovered"));
		DefaultCheckBoxStyle->CheckedHoveredImage.ImageSize = FVector2D(48.f, 48.f);
		DefaultCheckBoxStyle->CheckedHoveredImage.TintColor = FLinearColor::White;
		DefaultCheckBoxStyle->CheckedHoveredImage.SetResourceObject(CheckedHoveredMaterial);

		UObject* CheckedPressedMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/CheckBox/MI_CheckBox_Checked_Pressed"));
		DefaultCheckBoxStyle->CheckedPressedImage.ImageSize = FVector2D(48.f, 48.f);
		DefaultCheckBoxStyle->CheckedPressedImage.TintColor = FLinearColor::White;
		DefaultCheckBoxStyle->CheckedPressedImage.SetResourceObject(CheckedPressedMaterial);
	}

	WidgetStyle = *DefaultCheckBoxStyle;
}

// Protected Interfaces:

//~ Begin UWidget Interface
TSharedRef<SWidget> UJolfCheckBox::RebuildWidget()
{
	TSharedRef<SJolfCheckBox> MyJolfCheckBox = SAssignNew(MyCheckbox, SJolfCheckBox)
		.OnCheckStateChanged(BIND_UOBJECT_DELEGATE(FOnCheckStateChanged, SlateOnCheckStateChangedCallback))
		.Style(&WidgetStyle)
		.HAlign(HorizontalAlignment)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;

	if (GetChildrenCount() > 0)
	{
		MyCheckbox->SetContent(GetContentSlot()->Content ? GetContentSlot()->Content->TakeWidget() : SNullWidget::NullWidget);
	}

	return MyCheckbox.ToSharedRef();
}
//~ End UWidget Interface