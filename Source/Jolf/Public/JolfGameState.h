// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/GameStateBase.h"
#include "JolfGameState.generated.h"

UCLASS()
class JOLF_API AJolfGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	AJolfGameState(const FObjectInitializer& ObjectInitializer);

public: // Functions:

	void SetSequentialTurns(bool bInSequentialTurns);

	void SetEnablePawnCollision(bool bInEnablePawnCollision);
	void SetActivePlayerState(class AJolfPlayerState* InPlayerState);

	void SetWaitForAllPlayers(bool bInWaitForAllPlayers);
	void SetAllowStrokeWhileMoving(bool bInAllowStrokeWhileMoving);

public: // Interfaces

	//~ Begin UObject Interface
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin AActor Interface
	void PostInitializeComponents() override;
	//~ End AActor Interface

	//~ Begin AGameStateBase Interface
	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;
	//~ End AGameStateBase Interface

public: // Properties

	/** Warning: this list is not sorted! */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Jolf")
	TArray<class AJolfHole*> Holes;

	/** Should pawn collision response change to block after leaving tee? */
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_EnablePawnCollision, Category = "Jolf")
	uint8 bEnablePawnCollision:1;

	/** Should each player take turns moving? */
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_SequentialTurns, Category = "Jolf")
	uint8 bSequentialTurns:1;

	/** Should players wait until everyone finishes the hole before continuing? */
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_WaitForAllPlayers, Category = "Jolf")
	uint8 bShouldWaitForAllPlayersToFinishHole:1;

	/** Should players be allowed to submit a stroke while not idle? */
	UPROPERTY(VisibleInstanceOnly, ReplicatedUsing = OnRep_AllowStrokeWhileMoving, Category = "Jolf")
	uint8 bAllowStrokeWhileMoving:1;

	/** Number of times undo can be used without counting towards the score. */
	UPROPERTY(VisibleInstanceOnly, Transient, ReplicatedUsing = OnRep_NumFreeMulligans, Category = "Jolf")
	int32 NumFreeMulligans;

	UPROPERTY(VisibleInstanceOnly, Transient, ReplicatedUsing = OnRep_ActivePlayerState, Category = "Jolf")
	class AJolfPlayerState* ActivePlayerState;

	FSimpleMulticastDelegate OnEnablePawnCollisionChanged;
	FSimpleMulticastDelegate OnSequentialTurnsChanged;
	FSimpleMulticastDelegate OnWaitForAllPlayersChanged;
	FSimpleMulticastDelegate OnAllowStrokeWhileMovingChanged;
	FSimpleMulticastDelegate OnNumFreeMulligansChanged;

	DECLARE_EVENT_OneParam(AJolfGameState, FPlayerEvent, class AJolfPlayerState*);

	FPlayerEvent OnPlayerAdded;
	FPlayerEvent OnPlayerRemoved;

private: // Functions

	UFUNCTION()
	void OnRep_EnablePawnCollision();

	UFUNCTION()
	void OnRep_SequentialTurns();

	UFUNCTION()
	void OnRep_WaitForAllPlayers();

	UFUNCTION()
	void OnRep_ActivePlayerState();

	UFUNCTION()
	void OnRep_NumFreeMulligans();

	UFUNCTION()
	void OnRep_AllowStrokeWhileMoving();
};