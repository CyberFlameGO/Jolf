// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Components/Widget.h"
#include "JolfEditableTextBox.generated.h"

/** Based on Components/EditableTextBox.h */
UCLASS()
class JOLFUMG_API UJolfEditableTextBox : public UWidget
{
	GENERATED_BODY()

public:

	UJolfEditableTextBox(const FObjectInitializer& ObjectInitializer);

public: // Functions

	FText GetText() const;
	void SetText(FText InText);

public: // Properties

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEditableTextBoxChangedEvent, const FText&, Text);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEditableTextBoxCommittedEvent, const FText&, Text, ETextCommit::Type, CommitMethod);

	/** Called whenever the text is changed programmatically or interactively by the user */
	UPROPERTY(BlueprintAssignable, Category = "TextBox|Event")
	FOnEditableTextBoxChangedEvent OnTextChanged;

	/** Called whenever the text is committed.  This happens when the user presses enter or the text box loses focus. */
	UPROPERTY(BlueprintAssignable, Category = "TextBox|Event")
	FOnEditableTextBoxCommittedEvent OnTextCommitted;

	UPROPERTY(EditAnywhere, Category = Content)
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FEditableTextStyle EditableTextStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush BorderImageNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance)
	FSlateBrush BorderImageFocused;

public: // Interfaces

	//~ Begin UVisual Interface
	void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

protected: // Interfaces

	//~ Begin UWidget Interface
	TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

protected: // Properties

	TSharedPtr<class SJolfEditableTextBox> MyEditableTextBox;

private: // Functions

	void HandleOnTextChanged(const FText& InText);
	void HandleOnTextCommitted(const FText& InText, ETextCommit::Type CommitMethod);
};