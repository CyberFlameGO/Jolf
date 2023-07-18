// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "JolfSceneProbeSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class UJolfSceneProbeSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public: // Properties

	UPROPERTY(EditAnywhere, Config)
	TSubclassOf<class AJolfSceneProbeActor> ActorClass;
	
#if WITH_EDITORONLY_DATA
	/** Draw texture render target without any extra visualizations. */
	UPROPERTY(EditAnywhere, Config)
	TSoftObjectPtr<class UMaterialInterface> DebugRenderTargetMaterial;

	/** Visualize influence alpha of scene probe. */
	UPROPERTY(EditAnywhere, Config)
	TSoftObjectPtr<class UMaterialInterface> DebugAlphaMaterial;
#endif // WITH_EDITORONLY_DATA
};