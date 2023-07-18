// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "JolfSpectatorPawn.generated.h"

UCLASS()
class AJolfSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()

public:

	AJolfSpectatorPawn(const FObjectInitializer& ObjectInitializer);

public: // Interfaces

	//~ Begin AActor Interface
	void PostInitializeComponents() override;
	//~ End AActor Interface

protected: // Interfaces

	//~ Begin APawn Interface
	void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	//~ End APawn Interface

private: // Functions

	void OnPitchInput(float InAxisValue);
	void OnYawInput(float InAxisValue);
	void OnTurnPitchAtRateInput(float InAxisValue);
	void OnTurnYawAtRateInput(float InAxisValue);
	void OnFreecamSpeedAxisInput(float InAxisValue);
	void SynchronizeSpeed();

private: // Properties

	UPROPERTY(VisibleInstanceOnly)
	float NormalizedSpeed;

	UPROPERTY(EditAnywhere)
	float MinSpeed;
	
	UPROPERTY(EditAnywhere)
	float MaxSpeed;
};