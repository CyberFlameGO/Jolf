// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfAuthorityRulesMenu.h"
#include "JolfPlayerController.h"
#include "JolfGameState.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"
#include "JolfMenuStackRoot.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfAuthorityRulesMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(CollisionCheckBox))
		CollisionCheckBox->OnCheckStateChanged.AddDynamic(this, &UJolfAuthorityRulesMenu::OnCollisionCheckStateChanged);

	if (ensure(SequentialTurnsCheckBox))
		SequentialTurnsCheckBox->OnCheckStateChanged.AddDynamic(this, &UJolfAuthorityRulesMenu::OnSequentialTurnsCheckStateChanged);

	if (ensure(WaitForAllPlayersCheckBox))
		WaitForAllPlayersCheckBox->OnCheckStateChanged.AddDynamic(this, &UJolfAuthorityRulesMenu::OnWaitForAllPlayersCheckStateChanged);

	if (ensure(AllowStrokeWhileMovingCheckBox))
		AllowStrokeWhileMovingCheckBox->OnCheckStateChanged.AddDynamic(this, &UJolfAuthorityRulesMenu::OnAllowStrokeWhileMovingCheckStateChanged);

	if (ensure(NumFreeMulligansSpinBox))
		NumFreeMulligansSpinBox->OnValueCommitted.AddDynamic(this, &UJolfAuthorityRulesMenu::OnNumFreeMulligansValueCommitted);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfAuthorityRulesMenu::OnBackClicked);
}

void UJolfAuthorityRulesMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	check(World);
	if (AJolfGameState* GameState = World->GetGameState<AJolfGameState>())
	{
		BindGameState(GameState);
	}
	else
	{
		GameStateSetEventHandle = World->GameStateSetEvent.AddUObject(this, &UJolfAuthorityRulesMenu::OnGameStateSet);
	}
}

void UJolfAuthorityRulesMenu::NativeDestruct()
{
	UWorld* World = GetWorld();
	check(World);
	if (GameStateSetEventHandle.IsValid())
	{
		World->GameStateSetEvent.Remove(GameStateSetEventHandle);
		GameStateSetEventHandle.Reset();
	}

	if (AJolfGameState* GameState = World->GetGameState<AJolfGameState>())
	{
		GameState->OnEnablePawnCollisionChanged.RemoveAll(this);
		GameState->OnSequentialTurnsChanged.RemoveAll(this);
		GameState->OnWaitForAllPlayersChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfAuthorityRulesMenu::OnCollisionCheckStateChanged(bool bInIsChecked)
{
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->ServerSetPawnCollisionEnabled(bInIsChecked);
	}
}

void UJolfAuthorityRulesMenu::OnSequentialTurnsCheckStateChanged(bool bInIsChecked)
{
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->ServerSetSequentialTurnsEnabled(bInIsChecked);
	}
}

void UJolfAuthorityRulesMenu::OnWaitForAllPlayersCheckStateChanged(bool bInIsChecked)
{
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->ServerSetWaitForAllPlayers(bInIsChecked);
	}
}

void UJolfAuthorityRulesMenu::OnAllowStrokeWhileMovingCheckStateChanged(bool bInIsChecked)
{
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->ServerSetAllowStrokeWhileMoving(bInIsChecked);
	}
}

void UJolfAuthorityRulesMenu::OnNumFreeMulligansValueCommitted(float InValue, ETextCommit::Type CommitMethod)
{
	if (AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>())
	{
		PC->ServerSetNumFreeMulligans(FMath::RoundToInt(InValue));
	}
}

void UJolfAuthorityRulesMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfAuthorityRulesMenu::OnGameStateSet(AGameStateBase* InGameState)
{
	AJolfGameState* TypedGameState = Cast<AJolfGameState>(InGameState);
	if (ensure(TypedGameState))
	{
		GetWorld()->GameStateSetEvent.Remove(GameStateSetEventHandle);
		GameStateSetEventHandle.Reset();
		BindGameState(TypedGameState);
	}
}

void UJolfAuthorityRulesMenu::BindGameState(AJolfGameState* InGameState)
{
	InGameState->OnEnablePawnCollisionChanged.AddUObject(this, &UJolfAuthorityRulesMenu::OnEnablePawnCollisionChanged);
	if (ensure(CollisionCheckBox))
		CollisionCheckBox->SetIsChecked(InGameState->bEnablePawnCollision);

	InGameState->OnSequentialTurnsChanged.AddUObject(this, &UJolfAuthorityRulesMenu::OnSequentialTurnsChanged);
	if (ensure(SequentialTurnsCheckBox))
		SequentialTurnsCheckBox->SetIsChecked(InGameState->bSequentialTurns);

	InGameState->OnWaitForAllPlayersChanged.AddUObject(this, &UJolfAuthorityRulesMenu::OnWaitForAllPlayersChanged);
	if (ensure(WaitForAllPlayersCheckBox))
		WaitForAllPlayersCheckBox->SetIsChecked(InGameState->bShouldWaitForAllPlayersToFinishHole);

	InGameState->OnAllowStrokeWhileMovingChanged.AddUObject(this, &UJolfAuthorityRulesMenu::OnAllowStrokeWhileMovingChanged);
	if (ensure(AllowStrokeWhileMovingCheckBox))
		AllowStrokeWhileMovingCheckBox->SetIsChecked(InGameState->bAllowStrokeWhileMoving);

	InGameState->OnNumFreeMulligansChanged.AddUObject(this, &UJolfAuthorityRulesMenu::OnNumFreeMulligansChanged);
	if (ensure(NumFreeMulligansSpinBox))
		NumFreeMulligansSpinBox->SetValue(InGameState->NumFreeMulligans);
}

void UJolfAuthorityRulesMenu::OnEnablePawnCollisionChanged()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	check(GameState);
	if (ensure(CollisionCheckBox))
		CollisionCheckBox->SetIsChecked(GameState->bEnablePawnCollision);
}

void UJolfAuthorityRulesMenu::OnSequentialTurnsChanged()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	check(GameState);
	if (ensure(SequentialTurnsCheckBox))
		SequentialTurnsCheckBox->SetIsChecked(GameState->bSequentialTurns);
}

void UJolfAuthorityRulesMenu::OnWaitForAllPlayersChanged()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	check(GameState);
	if (ensure(WaitForAllPlayersCheckBox))
		WaitForAllPlayersCheckBox->SetIsChecked(GameState->bShouldWaitForAllPlayersToFinishHole);
}

void UJolfAuthorityRulesMenu::OnAllowStrokeWhileMovingChanged()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	check(GameState);
	if (ensure(AllowStrokeWhileMovingCheckBox))
		AllowStrokeWhileMovingCheckBox->SetIsChecked(GameState->bAllowStrokeWhileMoving);
}

void UJolfAuthorityRulesMenu::OnNumFreeMulligansChanged()
{
	AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>();
	check(GameState);
	if (ensure(NumFreeMulligansSpinBox))
		NumFreeMulligansSpinBox->SetValue(GameState->NumFreeMulligans);
}