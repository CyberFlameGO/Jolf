// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Components/CheckBox.h"
#include "JolfCheckBox.generated.h"

UCLASS()
class JOLFUMG_API UJolfCheckBox : public UCheckBox
{
	GENERATED_BODY()

public:

	UJolfCheckBox(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin UWidget Interface
	TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface
};