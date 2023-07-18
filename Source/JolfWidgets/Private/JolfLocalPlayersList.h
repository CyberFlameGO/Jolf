// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Blueprint/UserWidget.h"
#include "JolfLocalPlayersList.generated.h"

UCLASS(Abstract)
class UJolfLocalPlayersList : public UUserWidget
{
	GENERATED_BODY()

public: // Functions

	int32 GetNumPlayers() const { return NumPlayers; }
	void SetNumPlayers(int32 InNumPlayers);
	void SetPlayerSettings(int32 InControllerId, class UJolfPerPlayerSettings* InPlayerSettings);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

public: // Properties

	DECLARE_EVENT_OneParam(UJolfLocalPlayersList, FOnNumPlayersChangedEvent, int32);
	FOnNumPlayersChangedEvent OnNumPlayersChanged;

	DECLARE_EVENT_TwoParams(UJolfLocalPlayersList, FOnLocalPlayerClickedEvent, int32, class UJolfPerPlayerSettings*);
	FOnLocalPlayerClickedEvent OnLocalPlayerClicked;

private: // Functions

	UFUNCTION()
	void OnNumPlayersSpinBoxChanged(float InValue, ETextCommit::Type CommitMethod);

	void OnPlayerSettingsClicked(class UJolfPerPlayerSettings* InPlayerSettings);

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* NumPlayersSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class UVerticalBox* EntriesBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UJolfLocalPlayerEntry> EntryClass;

	UPROPERTY(Transient)
	TArray<class UJolfLocalPlayerEntry*> EntryInstances;

	UPROPERTY(Transient)
	int32 NumPlayers;
};
