// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfPawnCollisionEnabledTest.generated.h"

/**
 * Pawns should block each other when collision is enabled.
 */
UCLASS()
class AJolfPawnCollisionEnabledTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfPawnCollisionEnabledTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void PawnsOverlapAtSpawn();
	void PawnsBlockAfterMovement();

private: // Properties

	UPROPERTY()
	FTimerHandle TimerHandle;
};
