// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Framework/Commands/Commands.h" // Super

class FJolfModEditorCommands : public TCommands<FJolfModEditorCommands>
{
public: // Constructors

	FJolfModEditorCommands();

public: // Interfaces

	//~ Begin TCommands Interface
	void RegisterCommands() override;
	//~ End TCommands Interface

public: // Properties

	TSharedPtr<FUICommandInfo> CreateModCommand;
	TSharedPtr<FUICommandInfo> PackageModCommand;
};