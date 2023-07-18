// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfMenuStackRoot.h"
#include "JolfMenuStackContent.h"
#include "Components/WidgetSwitcher.h"
#include "JolfLocalPlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "JolfPlayerController.h"
#include "Slate/SObjectWidget.h"

UJolfMenuStackRoot::UJolfMenuStackRoot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsFocusable = true;
}

// Public Functions:

UJolfMenuStackContent* UJolfMenuStackRoot::FindMenuByClass(TSubclassOf<UJolfMenuStackContent> InClass) const
{
	for (int32 Index = WidgetSwitcher->GetChildrenCount() - 1; Index >= 0; --Index)
	{
		UWidget* Child = WidgetSwitcher->GetChildAt(Index);
		if (Child->IsA(InClass))
		{
			return (UJolfMenuStackContent*) Child;
		}
	}

	return nullptr;
}

void UJolfMenuStackRoot::SetActiveMenu(FName InMenuName)
{
	if (ActiveMenuName == InMenuName)
		return;

	if (UJolfMenuStackContent** Content = MenuMap.Find(InMenuName))
	{
		check(*Content);

		UJolfLocalPlayer* LP = GetOwningLocalPlayer<UJolfLocalPlayer>();
		check(LP);

		if (ActiveMenuName != NAME_None)
		{
			FJolfMenuStackItem StackItem;
			StackItem.MenuName = ActiveMenuName;
			StackItem.WidgetToFocus = LP->GetSlateUserFocus();
			HistoryStack.Push(StackItem);
		}

		ActiveMenuName = InMenuName;
		WidgetSwitcher->SetActiveWidget(*Content);

		if ((*Content)->bIsFocusable)
		{
			LP->SetSlateUserFocus(*Content);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Menu %s %s not focusable"), *InMenuName.ToString(), *GetNameSafe(*Content));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Menu %s not in map"), *InMenuName.ToString());
	}
}

void UJolfMenuStackRoot::PopMenu()
{
	if (HistoryStack.Num() > 0)
	{
		FJolfMenuStackItem StackItem = HistoryStack.Pop(false);
		UJolfMenuStackContent** Content = MenuMap.Find(StackItem.MenuName);
		check(Content);
		ActiveMenuName = StackItem.MenuName;
		WidgetSwitcher->SetActiveWidget(*Content);
		UJolfLocalPlayer* LP = GetOwningLocalPlayer<UJolfLocalPlayer>();
		check(LP);
		if (StackItem.WidgetToFocus)
		{
			LP->SetSlateUserFocus(StackItem.WidgetToFocus);
		}
		else
		{
			LP->SetSlateUserFocus(WidgetSwitcher->GetActiveWidget());
		}
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfMenuStackRoot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (ensure(WidgetSwitcher))
	{
		for (int32 Index = WidgetSwitcher->GetChildrenCount() - 1; Index >= 0; --Index)
		{
			UWidget* Child = WidgetSwitcher->GetChildAt(Index);
			if (UJolfMenuStackContent* TypedContent = Cast<UJolfMenuStackContent>(Child))
			{
				TypedContent->WeakRoot = this;
				MenuMap.Add(TypedContent->MenuName, TypedContent);
			}
		}

		SetActiveMenu(DefaultMenuName);
	}
}

FReply UJolfMenuStackRoot::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (UWidget* ActiveWidget = WidgetSwitcher->GetActiveWidget())
	{
		return FReply::Handled().SetUserFocus(ActiveWidget->TakeWidget(), InFocusEvent.GetCause(), false);
	}
	else
	{
		return FReply::Unhandled();
	}
}

void UJolfMenuStackRoot::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	// If root was removed from focus path try to restore previous focus.
	if (PreviousFocusPath.IsValid())
	{
		TSharedPtr<SObjectWidget> SafeGCWidget = MyGCWidget.Pin();
		if (SafeGCWidget.IsValid())
		{
			if (!NewWidgetPath.ContainsWidget(SafeGCWidget.ToSharedRef()))
			{
				UJolfLocalPlayer* LP = GetOwningLocalPlayer<UJolfLocalPlayer>();
				check(LP);
				LP->SetSlateUserFocus(PreviousFocusPath.GetLastWidget().Pin().ToSharedRef());
			}
		}
	}
}

FReply UJolfMenuStackRoot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const EUINavigationAction NavAction = FSlateApplication::Get().GetNavigationActionFromKey(InKeyEvent);
	if (NavAction == EUINavigationAction::Back)
	{
		if (HistoryStack.Num() > 0)
		{
			PopMenu();
			return FReply::Handled();
		}

		if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
		{
			PC->ClosePauseMenu();
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);	
}
//~ End UUserWidget Interface