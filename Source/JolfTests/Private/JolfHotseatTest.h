// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfHotseatTest.generated.h"

/** Input should be routed to hotseat player. */
UCLASS()
class AJolfHotseatTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfHotseatTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface

private: // Functions

	void FirstPlayerPressMouse();
	void FirstPlayerMoveMouse();
	void FirstPlayerPressMouseAgain();
	void FirstPlayerReleaseMouse();
	void AfterMoveFirstPlayer();
	void HotseatPlayerChanged();
	void SecondPlayerPressMouse();
	void SecondPlayerMoveMouse();
	void SecondPlayerPressMouseAgain();
	void SecondPlayerReleaseMouse();
	void AfterMoveSecondPlayer();

private: // Properties

	UPROPERTY(Transient)
	TWeakObjectPtr<class UJolfViewportClient> ViewportClient;

	UPROPERTY()
	FTimerHandle TimerHandle;
};
