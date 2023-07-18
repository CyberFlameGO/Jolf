// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfChangePlayerSettingsMenu.generated.h"

UCLASS(Abstract)
class UJolfChangePlayerSettingsMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()

public: // Functions:

	void SetActivePlayerSettings(class UJolfPerPlayerSettings* InPlayerSettings);
	
public: // Properties

	DECLARE_DELEGATE_OneParam(FOnSelectionChangedDelegate, class UJolfPerPlayerSettings*);
	FOnSelectionChangedDelegate OnSelectionChanged;

protected: // Interfaces
	
	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*)BackButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions

	UFUNCTION()
	void OnBackClicked();

	UFUNCTION()
	void OnCreateClicked();

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	void OnSelectPlayerSettingsClicked(class UJolfPerPlayerSettings* InPlayerSettings);
	void OnRemovePlayerSettingsClicked(class UJolfPerPlayerSettings* InPlayerSettings);
	void OnPlayerSettingsAdded(class UJolfPerPlayerSettings* InPlayerSettings);
	void OnPlayerSettingsRemoved(class UJolfPerPlayerSettings* InPlayerSettings);

private: // Properties
	
	UPROPERTY(Meta = (BindWidget))
	class UScrollBox* OptionsScrollBox;

	UPROPERTY(Meta = (BindWidget))
	class UJolfEditableTextBox* NameTextBox;

	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(Meta = (BindWidget))
	class UButton* CreateButton;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UJolfManagePlayerSettingsEntry> EntryClass;

	UPROPERTY(Transient)
	class UJolfPerPlayerSettings* ActivePlayerSettings;

	UPROPERTY(Transient)
	TArray<class UJolfManagePlayerSettingsEntry*> Entries;

	FDelegateHandle OnPlayerSettingsAddedHandle;
	FDelegateHandle OnPlayerSettingsRemovedHandle;
};