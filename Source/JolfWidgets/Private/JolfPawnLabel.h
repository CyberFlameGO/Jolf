// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfPawnLabel.generated.h"

UCLASS(Abstract)
class UJolfPawnLabel : public UUserWidget
{
	GENERATED_BODY()

public: // Properties

	UPROPERTY()
	TWeakObjectPtr<class AJolfPawn> WeakPawn;

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	//~ End UUserWidget Interface

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* NameTextBlock;
};