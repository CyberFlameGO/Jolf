// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfProjectionPanel.h"
#include "Layout/ArrangedChildren.h"
#include "JolfProjectionPanelSlot.h"

UJolfProjectionPanelSlot* UJolfProjectionPanel::AddChildToCanvas(UWidget* Content)
{
	return Cast<UJolfProjectionPanelSlot>(Super::AddChild(Content));
}

void UJolfProjectionPanel::TickLayout()
{
	ULocalPlayer* LP = GetOwningLocalPlayer();
	if (LP == nullptr || LP->ViewportClient == nullptr)
		return;

	FSceneViewProjectionData ProjectionData;
	if (!LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
		return;
	
	const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();

	for (UPanelSlot* PanelSlot : Slots)
	{
		UJolfProjectionPanelSlot* TypedSlot = CastChecked<UJolfProjectionPanelSlot>(PanelSlot);

		FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(TypedSlot->WorldLocation, 1.f));
		if (Result.W > 0.0f)
		{
			// the result of this will be x and y coords in -1..1 projection space
			const float RHW = 1.0f / Result.W;
			FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

			// Move from projection space to normalized 0..1 UI space
			const float NormalizedX = (PosInScreenSpace.X / 2.f) + 0.5f;
			const float NormalizedY = 1.f - (PosInScreenSpace.Y / 2.f) - 0.5f;

			TypedSlot->Anchor = FVector2D(NormalizedX, NormalizedY);
			TypedSlot->SynchronizeProperties();
		}
	}
}

// Public Interfaces:

//~ Begin UVisual Interface
void UJolfProjectionPanel::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyCanvas.Reset();
}
//~ End UVisual Interface

// Protected Interfaces:

//~ Begin UWidget Interface
TSharedRef<SWidget> UJolfProjectionPanel::RebuildWidget()
{
	MyCanvas = SNew(SConstraintCanvas);

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UJolfProjectionPanelSlot* TypedSlot = Cast<UJolfProjectionPanelSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyCanvas.ToSharedRef());
		}
	}

	return MyCanvas.ToSharedRef();
}
//~ End UWidget Interface

//~ Begin UPanelWidget Interface
UClass* UJolfProjectionPanel::GetSlotClass() const
{
	return UJolfProjectionPanelSlot::StaticClass();
}

void UJolfProjectionPanel::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (MyCanvas.IsValid())
	{
		CastChecked<UJolfProjectionPanelSlot>(InSlot)->BuildSlot(MyCanvas.ToSharedRef());
	}
}

void UJolfProjectionPanel::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyCanvas.IsValid() && InSlot->Content)
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MyCanvas->RemoveSlot(Widget.ToSharedRef());
		}
	}
}
//~ End UPanelWidget Interface
