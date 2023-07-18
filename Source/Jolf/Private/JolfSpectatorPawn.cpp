// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSpectatorPawn.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "JolfPerPlayerSettings.h"
#include "JolfPlayerController.h"

AJolfSpectatorPawn::AJolfSpectatorPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GetCollisionComponent()->InitSphereRadius(21.5f);

	bAddDefaultMovementBindings = false;
	NormalizedSpeed = 0.5f;
	MinSpeed = 100;
	MaxSpeed = 2000;
}

// Public Interfaces:

//~ Begin AActor Interface
void AJolfSpectatorPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SynchronizeSpeed();
}
//~ End AActor Interface

// Protected Interfaces:

//~ Begin APawn Interface
void AJolfSpectatorPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);
	
	InInputComponent->BindAxis(TEXT("MoveForward"), this, &ADefaultPawn::MoveForward);
	InInputComponent->BindAxis(TEXT("MoveRight"), this, &ADefaultPawn::MoveRight);
	InInputComponent->BindAxis(TEXT("Pitch"), this, &AJolfSpectatorPawn::OnPitchInput);
	InInputComponent->BindAxis(TEXT("Yaw"), this, &AJolfSpectatorPawn::OnYawInput);
	InInputComponent->BindAxis(TEXT("TurnPitchAtRate"), this, &AJolfSpectatorPawn::OnTurnPitchAtRateInput);
	InInputComponent->BindAxis(TEXT("TurnYawAtRate"), this, &AJolfSpectatorPawn::OnTurnYawAtRateInput);
	InInputComponent->BindAxis(TEXT("FreecamSpeed"), this, &AJolfSpectatorPawn::OnFreecamSpeedAxisInput);
}
//~ End APawn Interface

// Private Functions:

void AJolfSpectatorPawn::OnPitchInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);
		AddControllerPitchInput(InAxisValue * Settings->MousePitchSensitivity);
	}
}

void AJolfSpectatorPawn::OnYawInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);
		AddControllerYawInput(InAxisValue * Settings->MouseYawSensitivity);
	}
}

void AJolfSpectatorPawn::OnTurnPitchAtRateInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);
		AddControllerPitchInput(InAxisValue * GetWorld()->GetDeltaSeconds() * CustomTimeDilation * Settings->GamepadPitchDegreesPerSecond);
	}
}

void AJolfSpectatorPawn::OnTurnYawAtRateInput(float InAxisValue)
{
	if (AJolfPlayerController* PC = GetController<AJolfPlayerController>())
	{
		UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings();
		check(Settings);
		AddControllerYawInput(InAxisValue * GetWorld()->GetDeltaSeconds() * CustomTimeDilation * Settings->GamepadYawDegreesPerSecond);
	}
}

void AJolfSpectatorPawn::OnFreecamSpeedAxisInput(float InAxisValue)
{
	NormalizedSpeed = FMath::Clamp(NormalizedSpeed + InAxisValue, 0.f, 1.f);
	SynchronizeSpeed();
}

void AJolfSpectatorPawn::SynchronizeSpeed()
{
	CastChecked<UFloatingPawnMovement>(MovementComponent)->MaxSpeed = FMath::Lerp(MinSpeed, MaxSpeed, NormalizedSpeed);
}