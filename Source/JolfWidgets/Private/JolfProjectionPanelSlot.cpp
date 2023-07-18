// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfProjectionPanelSlot.h"
#include "JolfProjectionPanel.h"

UJolfProjectionPanelSlot::UJolfProjectionPanelSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Slot(nullptr)
{}

// Public Functions:

void UJolfProjectionPanelSlot::BuildSlot(TSharedRef<SConstraintCanvas> InCanvas)
{
	Slot = &InCanvas->AddSlot()
	[
		Content == nullptr ? SNullWidget::NullWidget : Content->TakeWidget()
	];

	SynchronizeProperties();
}

// Public Interfaces:

//~ Begin UVisual Interface
void UJolfProjectionPanelSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	Slot = nullptr;
}
//~ End UVisual Interface

//~ Begin UPanelSlot Interface
void UJolfProjectionPanelSlot::SynchronizeProperties()
{
	Slot->AutoSize(true);
	Slot->Anchors(FAnchors(Anchor.X, Anchor.Y, 0.f, 0.f));
}
//~ End UPanelSlot Interface
