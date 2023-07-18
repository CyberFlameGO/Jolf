// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfMenuStackContent.h"
#include "JolfAuthorityRulesMenu.generated.h"

UCLASS(Abstract)
class UJolfAuthorityRulesMenu : public UJolfMenuStackContent
{
	GENERATED_BODY()
	
protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
	//~ End UUserWidget Interface

	//~ Begin UJolfMenuStackContent Interface
	UWidget* GetDefaultWidgetToFocus() override { return (UWidget*)BackButton; }
	//~ End UJolfMenuStackContent Interface

private: // Functions:

	UFUNCTION()
	void OnCollisionCheckStateChanged(bool bInIsChecked);

	UFUNCTION()
	void OnSequentialTurnsCheckStateChanged(bool bInIsChecked);
	
	UFUNCTION()
	void OnWaitForAllPlayersCheckStateChanged(bool bInIsChecked);
	
	UFUNCTION()
	void OnAllowStrokeWhileMovingCheckStateChanged(bool bInIsChecked);

	UFUNCTION()
	void OnNumFreeMulligansValueCommitted(float InValue, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnBackClicked();

	void OnGameStateSet(class AGameStateBase* InGameState);
	void BindGameState(class AJolfGameState* InGameState);

	void OnEnablePawnCollisionChanged();
	void OnSequentialTurnsChanged();
	void OnWaitForAllPlayersChanged();
	void OnAllowStrokeWhileMovingChanged();
	void OnNumFreeMulligansChanged();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* CollisionCheckBox;
	
	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* SequentialTurnsCheckBox;

	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* WaitForAllPlayersCheckBox;
	
	UPROPERTY(Meta = (BindWidget))
	class UCheckBox* AllowStrokeWhileMovingCheckBox;

	UPROPERTY(Meta = (BindWidget))
	class USpinBox* NumFreeMulligansSpinBox;

	UPROPERTY(Meta = (BindWidget))
	class UButton* BackButton;

	FDelegateHandle GameStateSetEventHandle;
};