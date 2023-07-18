// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Widgets/Layout/SConstraintCanvas.h"
#include "Components/PanelWidget.h"
#include "JolfProjectionPanel.generated.h"

class UJolfProjectionPanelSlot;

UCLASS()
class UJolfProjectionPanel : public UPanelWidget
{
	GENERATED_BODY()

public: // Functions

	UJolfProjectionPanelSlot* AddChildToCanvas(UWidget* Content);

	void TickLayout();

public: // Interfaces

	//~ Begin UVisual Interface
	void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

protected: // Interfaces

	//~ Begin UWidget Interface
	TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	//~ Begin UPanelWidget Interface
	UClass* GetSlotClass() const override;
	void OnSlotAdded(UPanelSlot* Slot) override;
	void OnSlotRemoved(UPanelSlot* Slot) override;
	//~ End UPanelWidget Interface

private: // Properties

	TSharedPtr<class SConstraintCanvas> MyCanvas;
};
