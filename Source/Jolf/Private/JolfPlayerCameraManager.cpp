// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPlayerCameraManager.h"
#include "JolfPawn.h"
#include "JolfPowerArrow.h"
#include "JolfPerPlayerSettings.h"
#include "JolfPlayerController.h"

// Public Functions:

void AJolfPlayerCameraManager::SetPower(float InPower)
{
	if (PostProcessInstance)
	{
		PostProcessInstance->SetScalarParameterValue(TEXT("Power"), InPower);
	}
}

// Public Interfaces:

//~ Begin AActor Interface
void AJolfPlayerCameraManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PostProcessInstance = UMaterialInstanceDynamic::Create(PostProcessTemplate, this);
}
//~ End AActor Interface

//~ Begin APlayerCameraManager Interface
void AJolfPlayerCameraManager::ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ApplyCameraModifiers(DeltaTime, InOutPOV);

	AJolfPlayerController* PC = Cast<AJolfPlayerController>(GetOwningPlayerController());

	if (const UJolfPerPlayerSettings* Settings = PC->GetPlayerSettings())
	{
		InOutPOV.FOV = Settings->HorizontalFieldOfView;
	}

	if (AJolfPawn* Pawn = PC->GetPawn<AJolfPawn>())
	{
		if (AJolfPowerArrow* Arrow = Pawn->GetPowerArrow())
		{
			PostProcessInstance->SetVectorParameterValue(TEXT("PawnLocation"), Arrow->GetActorLocation());
		}
	}

	FPostProcessSettings PPSettings;
	PPSettings.AddBlendable(PostProcessInstance, 1.f);
	AddCachedPPBlend(PPSettings, 1.f);
}
//~ End APlayerCameraManager Interface