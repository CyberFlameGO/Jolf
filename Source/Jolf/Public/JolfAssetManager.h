// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Engine/AssetManager.h"
#include "JolfAssetManager.generated.h"

UCLASS()
class JOLF_API UJolfAssetManager : public UAssetManager
{
	GENERATED_BODY()

public: // Functions

	static UJolfAssetManager& Get();

	class UJolfLevelDefinition* FindLevelDefinitionByWorld(const class UWorld& InWorld) const;
	class UJolfLevelDefinition* FindLevelDefinitionByGuid(const FGuid& InUniqueId) const;

public: // Interfaces

	//~ Begin UObject Interface
	void PostInitProperties() override;
	//~ End UObject Interface

	//~ Begin UAssetManager Interface
	void StartInitialLoading() override;
#if WITH_EDITOR
	void RefreshPrimaryAssetDirectory(bool bForceRefresh) override;
#endif // WITH_EDITOR
	//~ End UAssetManager Interface

public: // Properties

	/** @see: UJolfLevelDefinition */
	static const FPrimaryAssetType LevelDefinitionType;

private: // Functions

	void OnNewPluginCreated(class IPlugin& InPlugin);

	void ScanModsForPrimaryAssets();
};