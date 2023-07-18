// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfWaitForAllPlayersEnabledTest.generated.h"

/**
 * Landing in the hole with WaitForAllPlayers enabled should cause the player to spectate until both players proceed to the next hole.
 */
UCLASS()
class AJolfWaitForAllPlayersEnabledTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfWaitForAllPlayersEnabledTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void TestPlayerWaited();
	void TestPlayersContinued();

private: // Properties

	UPROPERTY()
	FTimerHandle TimerHandle;
};