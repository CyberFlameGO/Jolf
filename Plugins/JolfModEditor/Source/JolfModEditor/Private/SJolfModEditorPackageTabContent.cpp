// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "SJolfModEditorPackageTabContent.h"
#include "Interfaces/IPluginManager.h"
#include "Widgets/Input/SButton.h"
#include "UATHelper/Public/IUATHelperModule.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "JolfModEditor"

void SJolfModEditorPackageTabContent::Construct(const FArguments& InArgs)
{
	TSharedRef<SScrollBox> ScrollBox = SNew(SScrollBox);

	for (TSharedRef<IPlugin> Plugin : IPluginManager::Get().GetEnabledPluginsWithContent())
	{
		if (Plugin->GetType() == EPluginType::Mod)
		{
			ScrollBox->AddSlot()
			[
				SNew(SButton)
				.Text(FText::AsCultureInvariant(Plugin->GetFriendlyName()))
				.OnClicked(this, &SJolfModEditorPackageTabContent::OnPackageClicked, Plugin->GetName())
			];
		}
	}

	ChildSlot
	[
		ScrollBox
	];
}

FReply SJolfModEditorPackageTabContent::OnPackageClicked(FString InPluginName)
{
	TSharedPtr<IPlugin> SelectedPlugin = IPluginManager::Get().FindPlugin(InPluginName);
	if (!SelectedPlugin.IsValid())
		return FReply::Unhandled();

	FString CommandLine = FString::Printf(
		TEXT("BuildCookRun -Project=\"%s\" -DLCName=%s -BasedOnReleaseVersion=Vanilla -SkipBuild -Cook -Stage -Pak -DLCIncludeEngineContent -TargetPlatform=Win64 -ClientConfig=Shipping -Manifests"),
		*FPaths::GetProjectFilePath(),
		*InPluginName
	);

	FText TaskName = FText::Format(LOCTEXT("PackagePluginTaskName", "Packaging {0}"), FText::FromString(SelectedPlugin->GetFriendlyName()));

	IUATHelperModule::Get().CreateUatTask(CommandLine, INVTEXT("Desktop"), TaskName,
		/*TaskShortName*/ TaskName, /*TaskIcon*/ nullptr);

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE