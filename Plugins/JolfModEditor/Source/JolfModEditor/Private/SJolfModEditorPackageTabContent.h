// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Widgets/SCompoundWidget.h"

class SJolfModEditorPackageTabContent : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SJolfModEditorPackageTabContent) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private: // Functions

	FReply OnPackageClicked(FString InPluginName);
};