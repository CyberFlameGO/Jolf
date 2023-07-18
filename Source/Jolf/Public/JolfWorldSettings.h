// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/WorldSettings.h"
#include "JolfWorldSettings.generated.h"

UCLASS()
class AJolfWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	AJolfWorldSettings(const FObjectInitializer& ObjectInitializer);

public: // Interfaces

	//~ Begin AActor Interface
#if WITH_EDITOR
	void CheckForErrors() override;
#endif // WITH_EDITOR
	//~ End AActor Interface
};