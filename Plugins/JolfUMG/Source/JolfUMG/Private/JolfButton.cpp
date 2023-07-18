// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfButton.h"
#include "SJolfButton.h"
#include "Components/ButtonSlot.h"

static FButtonStyle* DefaultButtonStyle = nullptr;

UJolfButton::UJolfButton(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	if (DefaultButtonStyle == nullptr)
	{
		// Copy UMG button style.
		DefaultButtonStyle = new FButtonStyle(WidgetStyle);
	
		UObject* NormalMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/MI_Button_Normal"));
		DefaultButtonStyle->Normal.ImageSize = FVector2D(64.f, 64.f);
		DefaultButtonStyle->Normal.SetResourceObject(NormalMaterial);

		UObject* HoveredMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/MI_Button_Hovered"));
		DefaultButtonStyle->Hovered.ImageSize = FVector2D(64.f, 64.f);
		DefaultButtonStyle->Hovered.SetResourceObject(HoveredMaterial);

		UObject* PressedMaterial = LoadObject<UObject>(nullptr, TEXT("/Game/Materials/UserInterface/MI_Button_Pressed"));
		DefaultButtonStyle->Pressed.ImageSize = FVector2D(64.f, 64.f);
		DefaultButtonStyle->Pressed.SetResourceObject(PressedMaterial);
	}

	WidgetStyle = *DefaultButtonStyle;
}

// Protected Interfaces:

//~ Begin UWidget Interface
TSharedRef<SWidget> UJolfButton::RebuildWidget()
{
	TSharedRef<SJolfButton> MyJolfButton = SAssignNew(MyButton, SJolfButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&WidgetStyle)
		.ClickMethod(ClickMethod)
		.TouchMethod(TouchMethod)
		.PressMethod(PressMethod)
		.IsFocusable(IsFocusable)
		;

	if (GetChildrenCount() > 0)
	{
		Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());
	}

	return MyButton.ToSharedRef();
}
//~ End UWidget Interface