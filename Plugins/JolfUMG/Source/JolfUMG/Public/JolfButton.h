// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Components/Button.h"
#include "JolfButton.generated.h"

UCLASS()
class JOLFUMG_API UJolfButton : public UButton
{
	GENERATED_BODY()

public:

	UJolfButton(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin UWidget Interface
	TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface
};