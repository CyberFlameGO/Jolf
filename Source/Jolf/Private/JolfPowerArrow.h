// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/Actor.h"
#include "JolfPowerArrow.generated.h"

UCLASS()
class AJolfPowerArrow : public AActor
{
	GENERATED_BODY()

public:

	AJolfPowerArrow(const FObjectInitializer& ObjectInitializer);

public: // Functions

	void SetPower(float InPower);
	void SetColorArrowMask();
	void SetShadowArrowMask();

private: // Properties

	UPROPERTY(EditAnywhere)
	float OffsetLength;

	UPROPERTY(EditAnywhere)
	float ArrowLength;
	
	UPROPERTY(EditAnywhere)
	float ArrowMinScale;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* LineSegmentComponent0;
	
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* LineSegmentComponent1;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* LineSegmentComponent2;
	
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* LineSegmentComponent3;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* TriangleComponent;
};