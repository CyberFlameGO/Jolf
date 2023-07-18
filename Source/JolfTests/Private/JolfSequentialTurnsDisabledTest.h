// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfSequentialTurnsDisabledTest.generated.h"

/**
 * All pawns should be able to move simultaneously while SequentialTurns is disabled.
 */
UCLASS()
class AJolfSequentialTurnsDisabledTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfSequentialTurnsDisabledTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void LaunchPawns();
	void TestPawnsMoved();

private: // Properties

	UPROPERTY()
	FTimerHandle TimerHandle;
};