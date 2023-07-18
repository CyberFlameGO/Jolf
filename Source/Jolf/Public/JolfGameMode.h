// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "JolfGameMode.generated.h"

UCLASS()
class JOLF_API AJolfGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AJolfGameMode(const FObjectInitializer& ObjectInitializer);

public: // Functions

	void SetEnablePawnCollision(bool bInEnablePawnCollision);
	void SetEnableSequentialTurns(bool bInEnableSequentialTurns);
	void SetWaitForAllPlayers(bool bInWaitForAllPlayers);
	void SetAllowStrokeWhileMoving(bool bInAllowStrokeWhileMoving);
	void RequestTravel(const class UJolfLevelDefinition& InLevelDef);

public: // Interfaces

	//~ Begin AActor Interface
	void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

	//~ Begin AGameModeBase Interface
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	void InitGameState() override;
	//~ End AGameModeBase Interface

private: // Functions

	class AJolfPlayerState* GetNextActivePlayerState();
};
