// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfThumbnailCamera.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "JolfLevelDefinition.h"

AJolfThumbnailCamera::AJolfThumbnailCamera(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsEditorOnlyActor = true;
	
	// Initially a capture component, but that does not have an editor preview because 
	// it does not implement GetEditorPreviewInfo.
	CameraComponent = CreateEditorOnlyDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	if (CameraComponent)
	{
		RootComponent = CameraComponent;
		CameraComponent->AspectRatio = 1.0f;
		CameraComponent->bConstrainAspectRatio = true;
	}
}

// Public Functions:

void AJolfThumbnailCamera::Capture()
{
	UWorld* World = GetWorld();
	if (!ensure(World))
		return;

	UJolfLevelDefinition* LevelDef = UJolfLevelDefinition::FindByWorld(*World);
	if (LevelDef == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot capture without level definition"));
		return;
	}

	// Temporary capture component with same settings as camera.
	USceneCaptureComponent2D* CaptureComponent = NewObject<USceneCaptureComponent2D>(this, NAME_None, RF_Transient);
	CaptureComponent->FOVAngle = CameraComponent->FieldOfView;
	CaptureComponent->OrthoWidth = CameraComponent->OrthoWidth;
	CaptureComponent->PostProcessSettings = CaptureComponent->PostProcessSettings;
	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;
	CaptureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
	CaptureComponent->SetupAttachment(CameraComponent);
	CaptureComponent->RegisterComponent();

	UTextureRenderTarget2D* TextureTarget = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
	TextureTarget->InitCustomFormat(512, 512, EPixelFormat::PF_B8G8R8A8, /*bInForceLinearGamma*/ false);

	CaptureComponent->TextureTarget = TextureTarget;
	CaptureComponent->CaptureScene();
	CaptureComponent->TextureTarget = nullptr;
	CaptureComponent->DestroyComponent();

	if (UTexture2D* ExistingTexture = LevelDef->Thumbnail.LoadSynchronous())
	{
		TextureTarget->UpdateTexture2D(ExistingTexture, ETextureSourceFormat::TSF_BGRA8, CTF_SRGB, nullptr);
		ExistingTexture->CompressionSettings = TextureCompressionSettings::TC_EditorIcon; // UserInterface2D
		ExistingTexture->PostEditChange(); // Funnily this is responsible for updating the asset's thumbnail.
		ExistingTexture->MarkPackageDirty();
		UE_LOG(LogTemp, Display, TEXT("Updated thumbnail texture %s"), *ExistingTexture->GetPathName());
	}
	else
	{
		IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
		FString PackageName;
		FString AssetName;
		AssetTools.CreateUniqueAssetName(GetWorld()->GetOutermost()->GetName(), TEXT("_Thumbnail"), PackageName, AssetName);

		UTexture2D* NewTexture = TextureTarget->ConstructTexture2D(CreatePackage(*PackageName), AssetName, LevelDef->GetMaskedFlags(), CTF_SRGB, nullptr);
		if (NewTexture)
		{
			NewTexture->CompressionSettings = TextureCompressionSettings::TC_EditorIcon; // UserInterface2D
			NewTexture->MipGenSettings = TextureMipGenSettings::TMGS_FromTextureGroup;
			NewTexture->LODGroup = TextureGroup::TEXTUREGROUP_UI;
			NewTexture->MarkPackageDirty();
			FAssetRegistryModule::AssetCreated(NewTexture);
			UE_LOG(LogTemp, Display, TEXT("Created thumbnail texture %s"), *PackageName);
	
			LevelDef->Thumbnail = NewTexture;
			LevelDef->MarkPackageDirty();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create thumbnail texture %s"), *PackageName);
		}
	}

	TextureTarget->MarkPendingKill();
}