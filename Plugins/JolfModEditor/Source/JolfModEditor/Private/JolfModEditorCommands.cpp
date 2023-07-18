// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfModEditorCommands.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "JolfModEditor"

FJolfModEditorCommands::FJolfModEditorCommands()
	: TCommands<FJolfModEditorCommands>(TEXT("JolfModEditorCommandsContext"), LOCTEXT("ContextDescription", "Jolf Mod Editor"), NAME_None, FEditorStyle::GetStyleSetName())
{}

// Public Interfaces:

//~ Begin TCommands Interface
void FJolfModEditorCommands::RegisterCommands()
{
	UI_COMMAND(CreateModCommand, "Create Mod", "Create a new mod plugin.", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(PackageModCommand, "Package Mod", "Package an existing mod plugin.", EUserInterfaceActionType::Button, FInputChord());
}
//~ End TCommands Interface

#undef LOCTEXT_NAMESPACE