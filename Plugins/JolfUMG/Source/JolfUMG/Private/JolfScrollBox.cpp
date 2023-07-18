// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfScrollBox.h"

static FScrollBarStyle* DefaultScrollBarStyle = nullptr;

UJolfScrollBox::UJolfScrollBox(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if (DefaultScrollBarStyle == nullptr)
	{
		// Copy UMG check box style.
		DefaultScrollBarStyle = new FScrollBarStyle(WidgetBarStyle);

		DefaultScrollBarStyle->HorizontalBottomSlotImage.DrawAs = ESlateBrushDrawType::NoDrawType;
		DefaultScrollBarStyle->HorizontalTopSlotImage.DrawAs = ESlateBrushDrawType::NoDrawType;
		DefaultScrollBarStyle->VerticalBottomSlotImage.DrawAs = ESlateBrushDrawType::NoDrawType;
		DefaultScrollBarStyle->VerticalTopSlotImage.DrawAs = ESlateBrushDrawType::NoDrawType;

		UObject* BackgroundTexture = LoadObject<UObject>(nullptr, TEXT("/Game/Textures/UserInterface/ScrollThumb/T_ScrollBar_Background_UI"));
		DefaultScrollBarStyle->VerticalBackgroundImage.ImageSize = FVector2D(32.f, 32.f);
		DefaultScrollBarStyle->VerticalBackgroundImage.Margin = FMargin(0.5f);
		DefaultScrollBarStyle->VerticalBackgroundImage.TintColor = FLinearColor::White;
		DefaultScrollBarStyle->VerticalBackgroundImage.SetResourceObject(BackgroundTexture);
		DefaultScrollBarStyle->VerticalBackgroundImage.DrawAs = ESlateBrushDrawType::Box;

		UObject* NormalMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/ScrollThumb/MI_ScrollThumb_Normal"));
		DefaultScrollBarStyle->NormalThumbImage.ImageSize = FVector2D(32.f, 32.f);
		DefaultScrollBarStyle->NormalThumbImage.Margin = FMargin(0.5f);
		DefaultScrollBarStyle->NormalThumbImage.TintColor = FLinearColor::White;
		DefaultScrollBarStyle->NormalThumbImage.SetResourceObject(NormalMaterial);
		DefaultScrollBarStyle->NormalThumbImage.DrawAs = ESlateBrushDrawType::Box;

		UObject* HoveredMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/ScrollThumb/MI_ScrollThumb_Hovered"));
		DefaultScrollBarStyle->HoveredThumbImage.ImageSize = FVector2D(32.f, 32.f);
		DefaultScrollBarStyle->HoveredThumbImage.Margin = FMargin(0.5f);
		DefaultScrollBarStyle->HoveredThumbImage.TintColor = FLinearColor::White;
		DefaultScrollBarStyle->HoveredThumbImage.SetResourceObject(HoveredMaterial);
		DefaultScrollBarStyle->HoveredThumbImage.DrawAs = ESlateBrushDrawType::Box;

		UObject* PressedMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/ScrollThumb/MI_ScrollThumb_Pressed"));
		DefaultScrollBarStyle->DraggedThumbImage.ImageSize = FVector2D(32.f, 32.f);
		DefaultScrollBarStyle->DraggedThumbImage.Margin = FMargin(0.5f);
		DefaultScrollBarStyle->DraggedThumbImage.TintColor = FLinearColor::White;
		DefaultScrollBarStyle->DraggedThumbImage.SetResourceObject(PressedMaterial);
		DefaultScrollBarStyle->DraggedThumbImage.DrawAs = ESlateBrushDrawType::Box;
	}

	WidgetBarStyle = *DefaultScrollBarStyle;

	ScrollbarThickness = FVector2D(32.f, 32.f);
	ScrollbarPadding = FVector2D::ZeroVector;
	AlwaysShowScrollbarTrack = true;
}