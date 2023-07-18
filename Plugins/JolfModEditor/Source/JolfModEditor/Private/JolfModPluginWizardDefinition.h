// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "IPluginWizardDefinition.h"

class FJolfModPluginWizardDefinition : public IPluginWizardDefinition
{
public:

	FJolfModPluginWizardDefinition();
	virtual ~FJolfModPluginWizardDefinition() {}

public: // Interfaces

	//~ Begin IPluginWizardDefinition Interface
	const TArray<TSharedRef<FPluginTemplateDescription>>& GetTemplatesSource() const override;
	void OnTemplateSelectionChanged(TArray<TSharedRef<FPluginTemplateDescription>> InSelectedItems, ESelectInfo::Type SelectInfo) override;
	bool HasValidTemplateSelection() const override;
	ESelectionMode::Type GetSelectionMode() const override;
	TArray<TSharedPtr<FPluginTemplateDescription>> GetSelectedTemplates() const override;
	void ClearTemplateSelection() override;
	bool AllowsEnginePlugins() const override;
	bool CanShowOnStartup() const override;
	bool CanContainContent() const override;
	bool HasModules() const override;
	bool IsMod() const override;
	void OnShowOnStartupCheckboxChanged(ECheckBoxState CheckBoxState) override;
	ECheckBoxState GetShowOnStartupCheckBoxState() const override;
	TSharedPtr<class SWidget> GetCustomHeaderWidget() override;
	FText GetInstructions() const override;
	bool GetPluginIconPath(FString& OutIconPath) const override;
	EHostType::Type GetPluginModuleDescriptor() const override;
	ELoadingPhase::Type GetPluginLoadingPhase() const override;
	bool GetTemplateIconPath(TSharedRef<FPluginTemplateDescription> InTemplate, FString& OutIconPath) const override;
	FString GetPluginFolderPath() const override;
	TArray<FString> GetFoldersForSelection() const override;
	void PluginCreated(const FString& PluginName, bool bWasSuccessful) const override;
	//~ End IPluginWizardDefinition Interface

private: // Properties

	TArray<TSharedRef<FPluginTemplateDescription>> TemplateDescriptions;
	TArray<TSharedRef<FPluginTemplateDescription>> SelectedTemplates;
};