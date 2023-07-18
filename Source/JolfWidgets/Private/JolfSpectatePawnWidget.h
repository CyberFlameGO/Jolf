// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfSpectatePawnWidget.generated.h"

UCLASS(Abstract)
class UJolfSpectatePawnWidget : public UUserWidget
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	//~ End UUserWidget Interface

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* ViewTargetTextBlock;
};