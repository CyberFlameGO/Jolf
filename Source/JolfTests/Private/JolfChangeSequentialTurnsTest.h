// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfChangeSequentialTurnsTest.generated.h"

/**
 * Disabling sequential turns should cause players to view their own pawn, and enabling should cause players to view the active pawn.
 */
UCLASS()
class AJolfChangeSequentialTurnsTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfChangeSequentialTurnsTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void TestDefaultSequentialTurnsEnabled();
	void TestSequentialTurnsDisabled();
	void TestSequentialTurnsEnabled();

private: // Properties

	UPROPERTY()
	FTimerHandle TimerHandle;
};