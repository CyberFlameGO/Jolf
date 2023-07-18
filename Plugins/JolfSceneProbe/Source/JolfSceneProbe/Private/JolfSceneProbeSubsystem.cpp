// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSceneProbeSubsystem.h"
#include "JolfSceneProbeSettings.h"
#include "JolfSceneProbeActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/World.h"

#if !UE_BUILD_SHIPPING
static bool bDrawSceneProbeBrushes = false;
static FAutoConsoleVariableRef CVarDrawSceneProbeBrushes(TEXT("j.SceneProbe.DrawBrushes"), bDrawSceneProbeBrushes, TEXT(""));
#endif // !UE_BUILD_SHIPPING

// Public Functions:

void UJolfSceneProbeSubsystem::AddBrushComponent(UPrimitiveComponent* InComponent)
{
	check(InComponent);
	check(ActorInstance);
	ActorInstance->AddBrushComponent(InComponent);
}

void UJolfSceneProbeSubsystem::RemoveBrushComponent(UPrimitiveComponent* InComponent)
{
	check(InComponent);
	check(ActorInstance);
	ActorInstance->RemoveBrushComponent(InComponent);
}

void UJolfSceneProbeSubsystem::UpdateHiddenComponents(TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
#if !UE_BUILD_SHIPPING
	if (bDrawSceneProbeBrushes)
		return;
#endif // !UE_BUILD_SHIPPING

	check(ActorInstance);
	for (TWeakObjectPtr<UPrimitiveComponent> WeakComponent : ActorInstance->GetCaptureComponent()->ShowOnlyComponents)
	{
		UPrimitiveComponent* Component = WeakComponent.Get();
		if (Component && Component->IsRegistered())
		{
			OutHiddenComponents.Add(Component->ComponentId);
		}
	}
}

// Public Interfaces

//~ Begin USubsystem Interface
bool UJolfSceneProbeSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	UWorld* World = Cast<UWorld>(Outer);
	check(World);
	return !World->IsNetMode(ENetMode::NM_DedicatedServer);
}

void UJolfSceneProbeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const UJolfSceneProbeSettings* Settings = GetDefault<UJolfSceneProbeSettings>();
	if (Settings->ActorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.ObjectFlags |= RF_Transient;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorInstance = GetWorld()->SpawnActor<AJolfSceneProbeActor>(Settings->ActorClass, SpawnParams);
	}
}
//~ End USubsystem Interface

// Protected Interfaces:

//~ Begin UWorldSubsystem Interface
bool UJolfSceneProbeSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}
//~ End UWorldSubsystem Interface