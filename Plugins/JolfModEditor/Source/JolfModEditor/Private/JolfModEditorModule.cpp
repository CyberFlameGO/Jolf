// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Modules/ModuleManager.h"
#include "JolfModEditorCommands.h"
#include "LevelEditor.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Docking/TabManager.h"
#include "IPluginBrowser.h"
#include "JolfModPluginWizardDefinition.h"
#include "Widgets/Docking/SDockTab.h"
#include "SJolfModEditorPackageTabContent.h"
#include "JolfModEditorStyle.h"

#define LOCTEXT_NAMESPACE "JolfModEditor"

class FJolfModEditorModule : public IModuleInterface
{
public: // Interfaces

	//~ Begin IModuleInterface
	void StartupModule() override;
	void ShutdownModule() override;
	//~ End IModuleInterface

private: // Functions

	TSharedRef<SDockTab> OnSpawnModCreatorTab(const FSpawnTabArgs& InSpawnTabArgs);
	TSharedRef<SDockTab> OnSpawnModPackagerTab(const FSpawnTabArgs& InSpawnTabArgs);
	void OnCreateModClicked();
	void OnPackageModClicked();
	void OnBuildMenu(FMenuBuilder& InBuilder);
	TSharedRef<SWidget> OnGetMenuContent();
	void OnMenuExtensionCallback(FMenuBuilder& InBuilder);
	void OnToolbarExtensionCallback(FToolBarBuilder& InBuilder);

private: // Properties

	FName ModCreatorTabName;
	FName ModPackagerTabName;
	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<FExtender> MenuExtender;
	TSharedPtr<FExtender> ToolbarExtender;
};

IMPLEMENT_MODULE(FJolfModEditorModule, JolfModEditor)

//~ Begin IModuleInterface
void FJolfModEditorModule::StartupModule()
{
	FJolfModEditorStyle::Initialize();
	FJolfModEditorCommands::Register();

	CommandList = MakeShared<FUICommandList>();
	CommandList->MapAction(FJolfModEditorCommands::Get().CreateModCommand, FExecuteAction::CreateRaw(this, &FJolfModEditorModule::OnCreateModClicked));
	CommandList->MapAction(FJolfModEditorCommands::Get().PackageModCommand, FExecuteAction::CreateRaw(this, &FJolfModEditorModule::OnPackageModClicked));

	MenuExtender = MakeShared<FExtender>();
	MenuExtender->AddMenuExtension(TEXT("FileProject"),
		EExtensionHook::After,
		CommandList,
		FMenuExtensionDelegate::CreateRaw(this, &FJolfModEditorModule::OnMenuExtensionCallback));

	ToolbarExtender = MakeShared<FExtender>();
	ToolbarExtender->AddToolBarExtension(TEXT("Compile"),
		EExtensionHook::After,
		CommandList,
		FToolBarExtensionDelegate::CreateRaw(this, &FJolfModEditorModule::OnToolbarExtensionCallback));

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);

	ModCreatorTabName = TEXT("ModCreator");
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ModCreatorTabName, FOnSpawnTab::CreateRaw(this, &FJolfModEditorModule::OnSpawnModCreatorTab))
		.SetMenuType(ETabSpawnerMenuType::Hidden)
		.SetAutoGenerateMenuEntry(false)
		.SetDisplayName(LOCTEXT("ModCreatorTabDisplayName", "New Mod"));

	ModPackagerTabName = TEXT("ModPackager");
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ModPackagerTabName, FOnSpawnTab::CreateRaw(this, &FJolfModEditorModule::OnSpawnModPackagerTab))
		.SetMenuType(ETabSpawnerMenuType::Hidden)
		.SetAutoGenerateMenuEntry(false)
		.SetDisplayName(LOCTEXT("ModPackagrTabDisplayName", "Package Mod"));
}

void FJolfModEditorModule::ShutdownModule()
{
	FJolfModEditorStyle::Shutdown();
	FJolfModEditorCommands::Unregister();

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(MenuExtender);
	LevelEditorModule.GetToolBarExtensibilityManager()->RemoveExtender(ToolbarExtender);

	FGlobalTabmanager::Get()->UnregisterTabSpawner(ModCreatorTabName);
	FGlobalTabmanager::Get()->UnregisterTabSpawner(ModPackagerTabName);
}
//~ End IModuleInterface

// Private Functions:

TSharedRef<SDockTab> FJolfModEditorModule::OnSpawnModCreatorTab(const FSpawnTabArgs& InSpawnTabArgs)
{
	return IPluginBrowser::Get().SpawnPluginCreatorTab(InSpawnTabArgs, MakeShared<FJolfModPluginWizardDefinition>());
}

TSharedRef<SDockTab> FJolfModEditorModule::OnSpawnModPackagerTab(const FSpawnTabArgs& InSpawnTabArgs)
{
	TSharedRef<SDockTab> NewTab = SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SJolfModEditorPackageTabContent)
	];

	return NewTab;
}

void FJolfModEditorModule::OnCreateModClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ModCreatorTabName);
}

void FJolfModEditorModule::OnPackageModClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ModPackagerTabName);
}

void FJolfModEditorModule::OnBuildMenu(FMenuBuilder& InBuilder)
{
	InBuilder.AddMenuEntry(FJolfModEditorCommands::Get().CreateModCommand);
	InBuilder.AddMenuEntry(FJolfModEditorCommands::Get().PackageModCommand);
}

TSharedRef<SWidget> FJolfModEditorModule::OnGetMenuContent()
{
	FMenuBuilder ModdingMenuBuilder(/*bInShouldCloseWindowAfterMenuSelection*/ true, CommandList);
	OnBuildMenu(ModdingMenuBuilder);
	return ModdingMenuBuilder.MakeWidget();
}

void FJolfModEditorModule::OnMenuExtensionCallback(FMenuBuilder& InBuilder)
{
	InBuilder.AddSubMenu(LOCTEXT("ToolbarLabel", "Modding"),
		LOCTEXT("ToolbarTooltip", "Create, package and share mods."),
		FNewMenuDelegate::CreateRaw(this, &FJolfModEditorModule::OnBuildMenu),
		false,
		FSlateIcon());
}

void FJolfModEditorModule::OnToolbarExtensionCallback(FToolBarBuilder& InBuilder)
{
	InBuilder.AddComboButton(FUIAction(),
		FOnGetContent::CreateRaw(this, &FJolfModEditorModule::OnGetMenuContent),
		LOCTEXT("ToolbarLabel", "Modding"),
		LOCTEXT("ToolbarTooltip", "Create, package and share mods."),
		FSlateIcon(FJolfModEditorStyle::Get().GetStyleSetName(), TEXT("LevelEditor.Modding")));
}

#undef LOCTEXT_NAMESPACE