// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfModPluginWizardDefinition.h"
#include "JolfModEditorSettings.h"
#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"

#define LOCTEXT_NAMESPACE "JolfModEditor"

FJolfModPluginWizardDefinition::FJolfModPluginWizardDefinition()
{
	FString PluginManagerBaseDir = IPluginManager::Get().FindPlugin(TEXT("PluginBrowser"))->GetBaseDir();
	FString EngineBlankPluginDir = PluginManagerBaseDir / TEXT("Templates") / TEXT("Blank");
	const FText BlankTemplateName = LOCTEXT("BlankTemplateName", "Blank");
	const FText BlankTemplateDesc = LOCTEXT("BlankTemplateDesc", "Create an empty mod without any default content.");
	TemplateDescriptions.Add(MakeShared<FPluginTemplateDescription>(BlankTemplateName, BlankTemplateDesc, EngineBlankPluginDir, /*InCanContainContent*/ true, EHostType::Runtime));

	FString PluginContentDir = IPluginManager::Get().FindPlugin(TEXT("JolfModEditor"))->GetContentDir();
	const FText FlatMapTemplateName = LOCTEXT("FlatMapTemplateName", "Flat Map");
	const FText FlatMapTemplateDesc = LOCTEXT("FlatMapTemplateDesc", "Create a mod with a flat grass course and single hole.");
	TemplateDescriptions.Add(MakeShared<FPluginTemplateDescription>(FlatMapTemplateName, FlatMapTemplateDesc, PluginContentDir / TEXT("FlatMapTemplate"), /*InCanContainContent*/ true, EHostType::Runtime));
}

//~ Begin IPluginWizardDefinition Interface
const TArray<TSharedRef<FPluginTemplateDescription>>& FJolfModPluginWizardDefinition::GetTemplatesSource() const { return TemplateDescriptions; }

void FJolfModPluginWizardDefinition::OnTemplateSelectionChanged(TArray<TSharedRef<FPluginTemplateDescription>> InSelectedItems, ESelectInfo::Type SelectInfo) 
{
	SelectedTemplates = InSelectedItems;
}

bool FJolfModPluginWizardDefinition::HasValidTemplateSelection() const { return true; }
ESelectionMode::Type FJolfModPluginWizardDefinition::GetSelectionMode() const { return ESelectionMode::Single; }

TArray<TSharedPtr<FPluginTemplateDescription>> FJolfModPluginWizardDefinition::GetSelectedTemplates() const 
{
	TArray<TSharedPtr<FPluginTemplateDescription>> SelectedTemplatePtrs;
	SelectedTemplatePtrs.Reserve(SelectedTemplates.Num());

	for (TSharedRef<FPluginTemplateDescription> SelectedTemplateRef : SelectedTemplates)
	{
		SelectedTemplatePtrs.Add(SelectedTemplateRef);
	}

	return SelectedTemplatePtrs;
}

void FJolfModPluginWizardDefinition::ClearTemplateSelection() 
{
	SelectedTemplates.Empty();
}

bool FJolfModPluginWizardDefinition::AllowsEnginePlugins() const { return false; }
bool FJolfModPluginWizardDefinition::CanShowOnStartup() const { return true;  }
bool FJolfModPluginWizardDefinition::CanContainContent() const { return true; }
bool FJolfModPluginWizardDefinition::HasModules() const { return false; }
bool FJolfModPluginWizardDefinition::IsMod() const { return true; }

void FJolfModPluginWizardDefinition::OnShowOnStartupCheckboxChanged(ECheckBoxState CheckBoxState) 
{
	UJolfModEditorSettings* Settings = GetMutableDefault<UJolfModEditorSettings>();
	Settings->bShowNewModWizardOnStartup = CheckBoxState == ECheckBoxState::Checked;
	Settings->SaveConfig();
}

ECheckBoxState FJolfModPluginWizardDefinition::GetShowOnStartupCheckBoxState() const 
{
	const UJolfModEditorSettings* Settings = GetDefault<UJolfModEditorSettings>();
	return Settings->bShowNewModWizardOnStartup ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

TSharedPtr<class SWidget> FJolfModPluginWizardDefinition::GetCustomHeaderWidget() { return nullptr; }

FText FJolfModPluginWizardDefinition::GetInstructions() const
{
	return LOCTEXT("Instructions", "Choose a template and then specify a name to create a new mod.");;
}

bool FJolfModPluginWizardDefinition::GetPluginIconPath(FString& OutIconPath) const { return false; }
EHostType::Type FJolfModPluginWizardDefinition::GetPluginModuleDescriptor() const { return EHostType::Runtime; }
ELoadingPhase::Type FJolfModPluginWizardDefinition::GetPluginLoadingPhase() const { return ELoadingPhase::Default; }
bool FJolfModPluginWizardDefinition::GetTemplateIconPath(TSharedRef<FPluginTemplateDescription> InTemplate, FString& OutIconPath) const { return false; }

FString FJolfModPluginWizardDefinition::GetPluginFolderPath() const
{
	if (SelectedTemplates.Num() > 0)
	{
		return SelectedTemplates[0]->OnDiskPath;
	}
	else
	{
		return FString();
	}
}

TArray<FString> FJolfModPluginWizardDefinition::GetFoldersForSelection() const
{ 
	TArray<FString> SelectedFolders;
	SelectedFolders.Reserve(SelectedTemplates.Num());

	for (TSharedRef<FPluginTemplateDescription> SelectedTemplate : SelectedTemplates)
	{
		SelectedFolders.Add(SelectedTemplate->OnDiskPath);
	}

	return SelectedFolders;
}

void FJolfModPluginWizardDefinition::PluginCreated(const FString& PluginName, bool bWasSuccessful) const 
{
	if (!bWasSuccessful)
		return;

	// Engine bug with AdditionalPluginDirectories crashes during startup because relative path is passed
	// to FProjectDescriptor::AddPluginDirectory. For this reason we remove the automatic "Mods" entry. No negative
	// side effects because Mods is a hardcoded special plugins directory.
	IProjectManager::Get().UpdateAdditionalPluginDirectory(FPaths::ProjectModsDir(), /*bAddOrRemove*/ false);
}
//~ End IPluginWizardDefinition Interface

#undef LOCTEXT_NAMESPACE
