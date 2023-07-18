// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfChangeWaitForAllPlayersTest.generated.h"

/**
 * Changing WaitForAllPlayers should cause players further ahead to spectate players further behind.
 */
UCLASS()
class AJolfChangeWaitForAllPlayersTest : public AJolfFunctionalTest 
{
	GENERATED_BODY()

public:

	AJolfChangeWaitForAllPlayersTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void TestPlayerWaited();
	void DisabledWaitForAllPlayers();
	void EnabledWaitForAllPlayers();
	void DisabledWaitForAllPlayersAgain();

private: // Properties

	UPROPERTY()
	FTimerHandle TimerHandle;
};