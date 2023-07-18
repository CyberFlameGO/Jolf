// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "JolfModEditorSettings.generated.h"

UCLASS(Config = EditorPerProjectUserSettings, DefaultConfig)
class UJolfModEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public: // Constructors

	UJolfModEditorSettings();

public: // Properties

	UPROPERTY(EditAnywhere, Config)
	uint32 bShowNewModWizardOnStartup:1;
};
