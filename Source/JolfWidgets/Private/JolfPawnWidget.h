// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfPawnWidget.generated.h"

UCLASS(Abstract)
class UJolfPawnWidget : public UUserWidget
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UJolfProjectionPanel* ProjectionCanvas;

	UPROPERTY(Meta = (BindWidget))
	class UImage* ArrowImage;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* StrengthTextBlock;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* HoleTextBlock;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* ParTextBlock;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* StrokeTextBlock;

	/** Displays active player name in hotseat mode. */
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* OwnerNameTextBlock;

	UPROPERTY(Transient)
	TArray<class UJolfPawnLabel*> PawnLabels;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UJolfPawnLabel> LabelClass;

	/** 3D label at the hole location. */
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* HoleMarkerTextBlock;

	UPROPERTY(Transient)
	class UMaterialInstanceDynamic* ArrowMaterial;
};