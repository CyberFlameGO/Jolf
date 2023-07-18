// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfProjectile.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "JolfMovementComponent.h"
#include "Components/StaticMeshComponent.h"

AJolfProjectile::AJolfProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetShouldUpdatePhysicsVolume(true);
	CollisionComponent->SetCanEverAffectNavigation(false);
	CollisionComponent->bDynamicObstacle = true;
	RootComponent = CollisionComponent;

	MovementComponent = CreateDefaultSubobject<UJolfMovementComponent>(TEXT("Movement"));
	MovementComponent->UpdatedComponent = CollisionComponent;
	MovementComponent->bAutoActivate = false;
	MovementComponent->bAutoUpdateTickRegistration = false;
}
