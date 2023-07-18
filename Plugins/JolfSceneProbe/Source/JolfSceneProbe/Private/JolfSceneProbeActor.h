// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/Actor.h"
#include "JolfSceneProbeActor.generated.h"

UCLASS()
class AJolfSceneProbeActor : public AActor
{
	GENERATED_BODY()

public:

	AJolfSceneProbeActor(const FObjectInitializer& ObjectInitializer);

public: // Functions

	void AddBrushComponent(class UPrimitiveComponent* InComponent);
	void RemoveBrushComponent(class UPrimitiveComponent* InComponent);

public: // Interfaces

	//~ Begin AActor Interface
	void PreInitializeComponents() override;
	void Tick(float DeltaSeconds) override;
	//~ End AActor Interface

public: // Functions

	class USceneCaptureComponent2D* GetCaptureComponent() const { return CaptureComponent; }

private: // Functions
	
	bool ShouldEnable() const;

private: // Properties

	UPROPERTY(EditAnywhere)
	class UMaterialParameterCollection* MaterialParameterCollection;

	UPROPERTY(Transient)
	class UMaterialParameterCollectionInstance* MPCInstance;

	UPROPERTY(VisibleDefaultsOnly)
	class USceneCaptureComponent2D* CaptureComponent;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* DebugRenderTargetComponent;
#endif // WITH_EDITORONLY_DATA

	UPROPERTY(Transient)
	bool bEnabled;
};