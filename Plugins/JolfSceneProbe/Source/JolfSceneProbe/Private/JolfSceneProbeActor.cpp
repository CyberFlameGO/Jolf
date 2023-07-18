// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSceneProbeActor.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "DrawDebugHelpers.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "JolfSceneProbeSettings.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/GameViewportClient.h"

static bool bEnableSceneProbe = true;
static FAutoConsoleVariableRef CVarEnableSceneProbe(TEXT("j.SceneProbe.Enable"), bEnableSceneProbe, TEXT(""));

#if ENABLE_DRAW_DEBUG
static bool bDrawSceneProbeCaptureBounds = false;
static FAutoConsoleVariableRef CVarSceneProbeDrawCaptureBounds(TEXT("j.SceneProbe.DrawCaptureBounds"), bDrawSceneProbeCaptureBounds, TEXT(""));
#endif // ENABLE_DRAW_DEBUG

#if WITH_EDITORONLY_DATA
static bool bDrawSceneProbeRenderTarget = false;
static FAutoConsoleVariableRef CVarDrawSceneProbeRender(TEXT("j.SceneProbe.DrawRenderTarget"), bDrawSceneProbeRenderTarget, TEXT(""));
static bool bDrawSceneProbeAlpha = false;
static FAutoConsoleVariableRef CVarDrawSceneProbeAlpha(TEXT("j.SceneProbe.DrawAlpha"), bDrawSceneProbeAlpha, TEXT(""));
#endif // WITH_EDITORONLY_DATA

AJolfSceneProbeActor::AJolfSceneProbeActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_DuringPhysics;
	
	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	RootComponent = CaptureComponent;
	CaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	CaptureComponent->OrthoWidth = 1000.f;
	CaptureComponent->ProfilingEventName = TEXT("JolfSceneProbe");
	CaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	CaptureComponent->ShowFlags.SetAtmosphere(false);
	CaptureComponent->ShowFlags.SetFog(false); // Otherwise fog shows up in the clear color.
	CaptureComponent->ShowFlags.SetBloom(false);
	CaptureComponent->ShowFlags.DisableAdvancedFeatures();
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorSceneDepth;
	CaptureComponent->bCaptureOnMovement = false; // Capture every frame instead.
	CaptureComponent->bCaptureEveryFrame = false; // Wait until tick before enabling.
}

// Public Functions:

void AJolfSceneProbeActor::AddBrushComponent(UPrimitiveComponent* InComponent)
{
	check(InComponent);
	CaptureComponent->ShowOnlyComponents.Add(InComponent);
	InComponent->SetVisibility(bEnabled);
}

void AJolfSceneProbeActor::RemoveBrushComponent(UPrimitiveComponent* InComponent)
{
	check(InComponent);
	CaptureComponent->ShowOnlyComponents.RemoveSingleSwap(InComponent);
}

// Public Interfaces:

//~ Begin AActor Interface
void AJolfSceneProbeActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	// UJolfSceneProbeSubsystem should not be instantiated on dedicated server.
	check(!IsNetMode(ENetMode::NM_DedicatedServer));
	
	// Brush component visibility update depends on bEnabled being initialized before AddBrushComponent is called.
	check(CaptureComponent->ShowOnlyComponents.Num() < 1);

	bEnabled = ShouldEnable();

	if (MaterialParameterCollection)
	{
		MPCInstance = GetWorld()->GetParameterCollectionInstance(MaterialParameterCollection);
	}
}

void AJolfSceneProbeActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// UJolfSceneProbeSubsystem should not be instantiated on dedicated server.
	check(!IsNetMode(ENetMode::NM_DedicatedServer));
	
	if (bEnabled != ShouldEnable())
	{
		bEnabled = !bEnabled;

		// Update visibility of brush components.
		CaptureComponent->ShowOnlyComponents.RemoveAllSwap([this](TWeakObjectPtr<UPrimitiveComponent> WeakComponent)
		{
			if (UPrimitiveComponent* Component = WeakComponent.Get())
			{
				Component->SetVisibility(bEnabled);
				return false; // Do not remove valid entry.
			}
			else
			{
				return true; // Remove null entry.
			}
		});

		if (!bEnabled)
		{
			// Clear any previously rendered brushes.
			UKismetRenderingLibrary::ClearRenderTarget2D(this, CaptureComponent->TextureTarget, CaptureComponent->TextureTarget->ClearColor);
		}
	}

	CaptureComponent->bCaptureEveryFrame = bEnabled;

	if (!bEnabled)
		return;

	if (CaptureComponent->TextureTarget == nullptr)
		return;

	UWorld* SceneProbeWorld = GetWorld();
	check(SceneProbeWorld);

	const UGameInstance* GameInstance = SceneProbeWorld->GetGameInstance();
	if (!ensure(GameInstance))
		return;

	APlayerController* TargetPlayerController = nullptr;
	for (ULocalPlayer* LocalPlayer : GameInstance->GetLocalPlayers())
	{
		if (LocalPlayer->Size.X < 0.5f || LocalPlayer->Size.Y < 0.5f)
		{
			// Hotseat player without viewport focus.
			continue;
		}

		if (LocalPlayer->PlayerController && ensure(LocalPlayer->PlayerController->GetWorld() == SceneProbeWorld))
		{
			TargetPlayerController = LocalPlayer->PlayerController;
			break;
		}
	}

	if (TargetPlayerController == nullptr)
		return;

	APawn* TargetPawn = TargetPlayerController->GetPawn();
	if (TargetPawn == nullptr)
		return;
			
	FVector PawnLocation = TargetPawn->GetActorLocation();

	// Snapping to pixel grid prevents flickering as grass blades move between pixels.
	int32 PixelSize = CaptureComponent->TextureTarget->SizeX;
	float CmPerPixel = CaptureComponent->OrthoWidth / PixelSize;
	PawnLocation.X = FMath::GridSnap(PawnLocation.X, CmPerPixel);
	PawnLocation.Y = FMath::GridSnap(PawnLocation.Y, CmPerPixel);

	FVector Location = PawnLocation + FVector(0.f, 0.f, 1000.f);
	SetActorLocationAndRotation(Location, FRotator(-90.f, 0.f, 90.f));

	if (ensure(MPCInstance))
	{
		static const FName ParameterName = TEXT("SceneProbe");
		const float InvOrthoWidth = 1.0f / CaptureComponent->OrthoWidth;
		MPCInstance->SetVectorParameterValue(ParameterName, FLinearColor(Location.X, Location.Y, Location.Z, InvOrthoWidth));
	}

#if ENABLE_DRAW_DEBUG
	if (bDrawSceneProbeCaptureBounds)
	{
		DrawDebugBox(SceneProbeWorld,
			PawnLocation,
			FVector(CaptureComponent->OrthoWidth * 0.5f, CaptureComponent->OrthoWidth * 0.5f, 1000.f),
			FColor::Blue,
			/*bPersistentLines*/ false,
			/*LifeTime*/ -1.f,
			/*DepthPriority*/ SDPG_World,
			/*Thickness*/ 5.f);
	}
#endif // ENABLE_DRAW_DEBUG

#if WITH_EDITORONLY_DATA
	if (bDrawSceneProbeRenderTarget || bDrawSceneProbeAlpha)
	{
		if (DebugRenderTargetComponent == nullptr)
		{	
			DebugRenderTargetComponent = NewObject<UStaticMeshComponent>(this, TEXT("DebugRenderTarget"));
			DebugRenderTargetComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane")));
			DebugRenderTargetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			DebugRenderTargetComponent->RegisterComponent();
		}

		const UJolfSceneProbeSettings* Settings = GetDefault<UJolfSceneProbeSettings>();
		if (bDrawSceneProbeAlpha)
		{
			DebugRenderTargetComponent->SetMaterial(0, Settings->DebugAlphaMaterial.LoadSynchronous());
		}
		else
		{
			DebugRenderTargetComponent->SetMaterial(0, Settings->DebugRenderTargetMaterial.LoadSynchronous());
		}

		float Scale = CaptureComponent->OrthoWidth * 0.01f;
		DebugRenderTargetComponent->SetWorldLocationAndRotationNoPhysics(PawnLocation, FRotator(0.f, 180.f, 0.f));
		DebugRenderTargetComponent->SetRelativeScale3D(FVector(Scale, Scale, 1.f));
	}
	else
	{
		if (DebugRenderTargetComponent)
		{
			DebugRenderTargetComponent->DestroyComponent();
			DebugRenderTargetComponent = nullptr;
		}
	}
#endif // WITH_EDITORONLY_DATA
}
//~ End AActor Interface

// Private Functions:

bool AJolfSceneProbeActor::ShouldEnable() const
{
	const UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ensure(ViewportClient) && ViewportClient->GetCurrentSplitscreenConfiguration() != ESplitScreenType::None)
	{
		// Disable during not-hotseat splitscreen because all pawns cannot necessarily fit nicely within capture region.
		return false;
	}

	return bEnableSceneProbe;
}