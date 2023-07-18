// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/Actor.h"
#include "JolfThumbnailCamera.generated.h"

/** Editor-only level thumbnail generator. */
UCLASS(HideCategories = (Actor, Collision, Cooking, Input, LOD, Rendering, Replication))
class AJolfThumbnailCamera : public AActor
{
	GENERATED_BODY()

public:

	AJolfThumbnailCamera(const FObjectInitializer& ObjectInitializer);

public: // Functions

	UFUNCTION(CallInEditor, Category = "Thumbnail")
	void Capture();

private: // Properties

	UPROPERTY(VisibleDefaultsOnly)
	class UCameraComponent* CameraComponent;
};