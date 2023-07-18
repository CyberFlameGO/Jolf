// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/Actor.h"
#include "JolfHole.generated.h"

UCLASS()
class AJolfHole : public AActor
{
	GENERATED_BODY()

public:

	AJolfHole(const FObjectInitializer& ObjectInitializer);

public: // Interfaces

	//~ Begin AActor Interface
	void PostInitializeComponents() override;
	void Destroyed() override;
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
#if WITH_EDITOR
	void CheckForErrors() override;
#endif // WITH_EDITOR
	//~ End AActor Interface

public: // Properties

	UPROPERTY(EditAnywhere)
	int32 HoleIndex;

	/** Number of strokes that a proficient player would require to complete this hole. */
	UPROPERTY(EditAnywhere)
	int32 Par;

private:

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* TriggerComponent;
};