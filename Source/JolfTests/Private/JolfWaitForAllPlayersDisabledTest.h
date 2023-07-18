// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfWaitForAllPlayersDisabledTest.generated.h"

/**
 * Landing in the hole with WaitForAllPlayers disabled should allow the player to continue to the next hole.
 */
UCLASS()
class AJolfWaitForAllPlayersDisabledTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfWaitForAllPlayersDisabledTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void TestPlayerDidNotWait();

private: // Properties

	UPROPERTY()
	FTimerHandle TimerHandle;
};