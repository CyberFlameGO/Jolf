// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/Actor.h"
#include "JolfProjectile.generated.h"

UCLASS()
class AJolfProjectile : public AActor
{
	GENERATED_BODY()

public:

	AJolfProjectile(const FObjectInitializer& ObjectInitializer);

public: // Functions

	class USphereComponent* GetCollisionComponent() const { return CollisionComponent; }
	class UJolfMovementComponent* GetGolfMovementComponent() const { return MovementComponent; }

private: // Properties

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleDefaultsOnly)
	class UJolfMovementComponent* MovementComponent;
};