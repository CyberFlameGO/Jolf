// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPlayerStart.h"
#include "Components/CapsuleComponent.h"

#if WITH_EDITORONLY_DATA
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/ArrowComponent.h"
#endif // WITH_EDITORONLY_DATA

#if WITH_EDITOR
#include "JolfHole.h"
#include "EngineUtils.h"
#include "Misc/UObjectToken.h"
#include "Logging/TokenizedMessage.h"
#include "Logging/MessageLog.h"
#endif // WITH_EDITOR

AJolfPlayerStart::AJolfPlayerStart(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.DoNotCreateDefaultSubobject(TEXT("Sprite")) // GoodSprite
		.DoNotCreateDefaultSubobject(TEXT("Sprite2")) // BadSprite
	)
{
	GetCapsuleComponent()->InitCapsuleSize(21.5f, 21.5f); // Sphere the same size as ball.

#if WITH_EDITORONLY_DATA
	if (!IsRunningCommandlet())
	{
		if (UArrowComponent* Arrow = GetArrowComponent())
		{
			Arrow->ArrowSize = 1.5f;
		}

		static ConstructorHelpers::FObjectFinder<UStaticMesh> EditorBallMesh(TEXT("/Game/Balls/Default/SM_DefaultBall"));

		MeshComponent = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
		if (MeshComponent)
		{
			MeshComponent->SetStaticMesh(EditorBallMesh.Object);
			MeshComponent->bHiddenInGame = true;
			MeshComponent->SetupAttachment(RootComponent);
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
#endif // WITH_EDITORONLY_DATA
}

// Public Interfaces:

//~ Begin AActor Interface
#if WITH_EDITOR
void AJolfPlayerStart::CheckForErrors()
{
	Super::CheckForErrors();
	
	AJolfHole* PairedGoal = nullptr;
	for (TActorIterator<AJolfHole> Iter(GetWorld()); Iter; ++Iter)
	{
		if (Iter->HoleIndex == HoleIndex)
		{
			PairedGoal = *Iter;
			break;
		}
	}

	if (PairedGoal == nullptr)
	{
		FMessageLog("MapCheck").Error()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(NSLOCTEXT("Jolf", "MapCheck_PlayerStartWithoutHole", "This player start does not have a paired hole.")));
	}
}
#endif // WITH_EDITOR
//~ End AActor Interface