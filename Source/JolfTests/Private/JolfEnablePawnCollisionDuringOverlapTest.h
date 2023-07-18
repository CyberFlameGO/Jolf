// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfEnablePawnCollisionDuringOverlapTest.generated.h"

/**
 * Enabling pawn collision during overlap should not block until finished overlapping.
 */
UCLASS()
class AJolfEnablePawnCollisionDuringOverlapTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfEnablePawnCollisionDuringOverlapTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void PawnsOverlapWhileDisabled();
	void PawnsOverlapAfterEnabling();
	void PawnsBlockAfterMovement();

private: // Properties

	UPROPERTY()
	FTimerHandle TimerHandle;
};
