// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "JolfSceneProbeSubsystem.generated.h"

/**
 * Manages per-world AJolfSceneProbeActor.
 * Only instantiated in game worlds not running as dedicated server.
 */
UCLASS()
class JOLFSCENEPROBE_API UJolfSceneProbeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public: // Functions

	/** Note component visibility is managed by scene probe. */
	void AddBrushComponent(class UPrimitiveComponent* InComponent);
	void RemoveBrushComponent(class UPrimitiveComponent* InComponent);
	void UpdateHiddenComponents(TSet<FPrimitiveComponentId>& OutHiddenComponents);

public: // Interfaces

	//~ Begin USubsystem Interface
	bool ShouldCreateSubsystem(UObject* Outer) const override;
	void Initialize(FSubsystemCollectionBase& Collection) override;
	//~ End USubsystem Interface

protected: // Interfaces

	//~ Begin UWorldSubsystem Interface
	bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	//~ End UWorldSubsystem Interface

private: // Properties

	UPROPERTY()
	class AJolfSceneProbeActor* ActorInstance;
};