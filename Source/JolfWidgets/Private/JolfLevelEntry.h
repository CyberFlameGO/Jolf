// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfLevelEntry.generated.h"

UCLASS(Abstract)
class UJolfLevelEntry : public UUserWidget
{
	GENERATED_BODY()

public:

	UJolfLevelEntry(const FObjectInitializer& ObjectInitializer);

public: // Functions

	class UJolfLevelDefinition* GetLevelDef() const { return LevelDef; }
	void SetLevelDef(class UJolfLevelDefinition* InLevelDef);

public: // Properties

	DECLARE_MULTICAST_DELEGATE_OneParam(FClicked, class UJolfLevelDefinition*);
	FClicked OnClicked;

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnButtonClicked();

private: // Properties

	UPROPERTY(Transient)
	class UJolfLevelDefinition* LevelDef;

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;

	UPROPERTY(Meta = (BindWidget))
	class UImage* ThumbnailImage;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* TitleTextBlock;
};