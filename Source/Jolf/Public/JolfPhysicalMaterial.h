// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "PhysicalMaterials/PhysicalMaterial.h"
#include "JolfPhysicalMaterial.generated.h"

UCLASS()
class UJolfPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public: // Properties

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	/** Spawned on impact. */
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* SplashSystem;

	/** Played on impact. */
	UPROPERTY(EditAnywhere)
	class USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve ImpactSpeedVolume;
};