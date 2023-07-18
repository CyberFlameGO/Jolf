// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfAssetManager.h"
#include "JolfLevelDefinition.h"
#include "Engine/World.h"
#include "Interfaces/IPluginManager.h"
#include "JolfLevelDefinition.h"

static bool bLogAssetManagerMods = false;
#if !NO_LOGGING
static FAutoConsoleVariableRef CVarLogAssetManagerMods(TEXT("j.LogAssetManagerMods"), bLogAssetManagerMods, TEXT(""));
#endif // !NO_LOGGING

// Public Functions:

UJolfAssetManager& UJolfAssetManager::Get()
{
	return *CastChecked<UJolfAssetManager>(&UAssetManager::Get());
}

UJolfLevelDefinition* UJolfAssetManager::FindLevelDefinitionByWorld(const UWorld& InWorld) const
{
	TArray<FAssetData> Assets;
	GetPrimaryAssetDataList(LevelDefinitionType, Assets);

#if WITH_EDITOR
	if (InWorld.WorldType == EWorldType::PIE)
	{
		FString PathString = InWorld.GetPathName(); // e.g. /Game/Maps/UEDPIE_0_Devtest.Devtest
		FSoftObjectPath Path = FSoftObjectPath(UWorld::RemovePIEPrefix(PathString)); // e.g. /Game/Maps/Devtest.Devtest

		for (FAssetData& Asset : Assets)
		{
			UJolfLevelDefinition* LevelDef = Cast<UJolfLevelDefinition>(Asset.GetAsset());
			if (LevelDef && LevelDef->Level.ToSoftObjectPath() == Path)
			{
				return LevelDef;
			}
		}

		return nullptr;
	}
#endif // WITH_EDITOR

	for (FAssetData& Asset : Assets)
	{
		UJolfLevelDefinition* LevelDef = Cast<UJolfLevelDefinition>(Asset.GetAsset());
		if (LevelDef && LevelDef->Level.Get() == &InWorld)
		{
			return LevelDef;
		}
	}

	return nullptr;
}

UJolfLevelDefinition* UJolfAssetManager::FindLevelDefinitionByGuid(const FGuid& InUniqueId) const
{
	if (InUniqueId.IsValid())
	{
		TArray<FAssetData> Assets;
		GetPrimaryAssetDataList(LevelDefinitionType, Assets);
		for (FAssetData& Asset : Assets)
		{
			UJolfLevelDefinition* LevelDef = Cast<UJolfLevelDefinition>(Asset.GetAsset());
			if (LevelDef && LevelDef->LevelGuid == InUniqueId)
			{
				return LevelDef;
			}
		}
	}

	return nullptr;
}

// Public Interfaces:

//~ Begin UObject Interface
void UJolfAssetManager::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(EObjectFlags::RF_ClassDefaultObject))
	{
		// UAssetManager binds events here as well.
		IPluginManager::Get().OnNewPluginCreated().AddUObject(this, &UJolfAssetManager::OnNewPluginCreated);
	}
}
//~ End UObject Interface

//~ Begin UAssetManager Interface
void UJolfAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UE_CLOG(bLogAssetManagerMods, LogTemp, Display, TEXT("StartInitialLoading triggering mod scan"));
	ScanModsForPrimaryAssets();
}
#if WITH_EDITOR
void UJolfAssetManager::RefreshPrimaryAssetDirectory(bool bForceRefresh)
{
	bool bShouldScan = bForceRefresh || !bIsPrimaryAssetDirectoryCurrent;

	Super::RefreshPrimaryAssetDirectory(bForceRefresh);

	if (bShouldScan)
	{
		UE_CLOG(bLogAssetManagerMods, LogTemp, Display, TEXT("RefreshPrimaryAssetDirectory triggering mod scan"));
		ScanModsForPrimaryAssets();
	}
}
#endif // WITH_EDITOR
//~ End UAssetManager Interface

// Public Properties:

const FPrimaryAssetType UJolfAssetManager::LevelDefinitionType(TEXT("LevelDefinition"));

// Private Functions:

void UJolfAssetManager::OnNewPluginCreated(IPlugin& InPlugin)
{
	if (InPlugin.GetType() != EPluginType::Mod)
		return;

	// We do not expect to actually find assets here yet, but scanning ahead of time means newly created level
	// definitions will be found without restarting the editor.

	// Danger: trailing forward slash on Path will only search subdirectories. Sigh.
	FString SearchPath = InPlugin.GetMountedAssetPath() / TEXT("Maps");

	UE_CLOG(bLogAssetManagerMods, LogTemp, Display, TEXT("Scanning new mod %s content: %s"), *InPlugin.GetFriendlyName(), *SearchPath);
	ScanPathForPrimaryAssets(LevelDefinitionType, SearchPath, UJolfLevelDefinition::StaticClass(), /*bHasBlueprintClasses*/ false, /*bIsEditorOnly*/ false, /*bForceSynchronousScan*/ false);
}

void UJolfAssetManager::ScanModsForPrimaryAssets()
{
	StartBulkScanning();

	TArray<FString> SearchPaths;
	for (TSharedRef<IPlugin> Plugin : IPluginManager::Get().GetEnabledPluginsWithContent())
	{
		if (Plugin->GetType() == EPluginType::Mod)
		{
			// Danger: trailing forward slash on Path will only search subdirectories. Sigh.
			SearchPaths.Emplace(Plugin->GetMountedAssetPath() / TEXT("Maps"));
		}
	}

	if (SearchPaths.Num() > 0)
	{
		// Return value is not useful to log because the actual search is deferred and asynchronous.
		ScanPathsForPrimaryAssets(LevelDefinitionType, SearchPaths, UJolfLevelDefinition::StaticClass(), /*bHasBlueprintClasses*/ false, /*bIsEditorOnly*/ false, /*bForceSynchronousScan*/ false);
	}

#if !NO_LOGGING
	UE_CLOG(bLogAssetManagerMods, LogTemp, Display, TEXT("Scanning %i mod(s) content:"), SearchPaths.Num());
	for (const FString& Path : SearchPaths)
	{
		UE_CLOG(bLogAssetManagerMods, LogTemp, Display, TEXT("\t%s"), *Path);
	}
#endif // !NO_LOGGING

	StopBulkScanning();
}