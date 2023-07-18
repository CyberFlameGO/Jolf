// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfMenuStackRoot.generated.h"

struct FJolfMenuStackItem
{
	FName MenuName;
	TSharedPtr<SWidget> WidgetToFocus;
};

UCLASS(Abstract)
class UJolfMenuStackRoot : public UUserWidget
{
	GENERATED_BODY()

public:

	UJolfMenuStackRoot(const FObjectInitializer& ObjectInitializer);

public: // Functions

	class UJolfMenuStackContent* FindMenuByClass(TSubclassOf<class UJolfMenuStackContent> InClass) const;
	void SetActiveMenu(FName InMenuName);
	void PopMenu();

	template<typename T>
	T* FindMenuByClass() const { return (T*) FindMenuByClass(T::StaticClass()); }

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;
	FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	//~ End UUserWidget Interface

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(Transient)
	TMap<FName, class UJolfMenuStackContent*> MenuMap;

	TArray<FJolfMenuStackItem> HistoryStack;

	UPROPERTY(Transient)
	FName ActiveMenuName;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultMenuName;
};