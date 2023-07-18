// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "JolfPlayerCameraManager.generated.h"

UCLASS()
class AJolfPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public: // Functions

	void SetPower(float InPower);

public: // Interfaces

	//~ Begin AActor Interface
	void PostInitializeComponents() override;
	//~ End AActor Interface

	//~ Begin APlayerCameraManager Interface
	void ApplyCameraModifiers(float DeltaTime, FMinimalViewInfo& InOutPOV) override;
	//~ End APlayerCameraManager Interface

private: // Properties

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* PostProcessTemplate;

	UPROPERTY(VisibleInstanceOnly, Transient)
	class UMaterialInstanceDynamic* PostProcessInstance;
};