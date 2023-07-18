// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfSequentialTurnsEnabledTest.generated.h"

/**
 * Only the active pawn should be able to move and be view target when SequentialTurns is enabled.
 */
UCLASS()
class AJolfSequentialTurnsEnabledTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfSequentialTurnsEnabledTest(const FObjectInitializer& ObjectInitializer);

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