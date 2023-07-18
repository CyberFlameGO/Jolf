// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Components/SpinBox.h"
#include "JolfSpinBox.generated.h"

UCLASS()
class JOLFUMG_API UJolfSpinBox : public USpinBox
{
	GENERATED_BODY()

public:

	UJolfSpinBox(const FObjectInitializer& ObjectInitializer);

public: // Interfaces

	//~ Begin UWidget Interface
	void SynchronizeProperties() override;
	//~ End UWidget Interface

public: // Properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush BackgroundImageSelected;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush CaretImage;

	/** Marked DesignerRebuild because interface units member is const, so it cannot be changed in SynchronizeProperties. */
	UPROPERTY(EditAnywhere, Category = Appearance, Meta = (DesignerRebuild))
	EUnit UnderlyingUnits;

protected: // Interfaces

	//~ Begin UWidget Interface
	TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface
};