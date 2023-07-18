// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Widgets/Layout/SConstraintCanvas.h"
#include "Components/PanelSlot.h"
#include "JolfProjectionPanelSlot.generated.h"

UCLASS()
class UJolfProjectionPanelSlot : public UPanelSlot
{
	GENERATED_BODY()

public:

	UJolfProjectionPanelSlot(const FObjectInitializer& ObjectInitializer);

public: // Functions

	void BuildSlot(TSharedRef<SConstraintCanvas> InCanvas);

public: // Interfaces

	//~ Begin UVisual Interface
	void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	//~ Begin UPanelSlot Interface
	void SynchronizeProperties() override;
	//~ End UPanelSlot Interface

public: // Properties

	UPROPERTY()
	FVector WorldLocation;

	UPROPERTY()
	FVector2D Anchor;

private: // Properties

	SConstraintCanvas::FSlot* Slot;
};
