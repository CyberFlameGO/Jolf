// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfMenuStackContent.generated.h"

UCLASS(Abstract)
class UJolfMenuStackContent : public UUserWidget
{
	GENERATED_BODY()

public:

	UJolfMenuStackContent(const FObjectInitializer& ObjectInitializer);

public: // Functions

	class UJolfMenuStackRoot* GetMenuRoot() const;

public: // Properties

	UPROPERTY(EditDefaultsOnly)
	FName MenuName;
	
	UPROPERTY(Transient)
	TWeakObjectPtr<class UJolfMenuStackRoot> WeakRoot;

protected: // Functions

	virtual UWidget* GetDefaultWidgetToFocus() { return nullptr; }

protected: // Interfaces

	//~ Begin UUserWidget Interface
	FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	//~ End UUserWidget Interface
};