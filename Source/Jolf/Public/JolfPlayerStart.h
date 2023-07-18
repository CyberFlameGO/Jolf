// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/PlayerStart.h"
#include "JolfPlayerStart.generated.h"

UCLASS()
class AJolfPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:

	AJolfPlayerStart(const FObjectInitializer& ObjectInitializer);

public: // Interfaces

	//~ Begin AActor Interface
#if WITH_EDITOR
	void CheckForErrors() override;
#endif // WITH_EDITOR
	//~ End AActor Interface

public: // Properties

	UPROPERTY(EditAnywhere)
	int32 HoleIndex;

private: // Properties

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UStaticMeshComponent* MeshComponent;
#endif // WITH_EDITORONLY_DATA
};