// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfPawnCollisionDisabledTest.generated.h"

/**
 * Pawns should move through each other when collision is disabled.
 */
UCLASS()
class AJolfPawnCollisionDisabledTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfPawnCollisionDisabledTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void PawnsOverlapAtSpawn();
	void PawnsOverlapAfterMovement();

private: // Properties

	UPROPERTY()
	FTimerHandle TimerHandle;
};