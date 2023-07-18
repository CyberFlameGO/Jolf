// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfModEditorStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyle.h"

TSharedPtr<FSlateStyleSet> FJolfModEditorStyle::StyleInstance = nullptr;

void FJolfModEditorStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FJolfModEditorStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<FSlateStyleSet> FJolfModEditorStyle::Create()
{
	// Unfortunately Unreal does not load the 40x40 version of this image, so we need a custom style.
	// SlateEditorStyle.cpp sets ContentRoot to FPaths::EngineContentDir() / TEXT("Editor/Slate")
	TSharedRef<FSlateStyleSet> Style = MakeShared<FSlateStyleSet>(TEXT("JolfModEditorStyle"));
	Style->Set(TEXT("LevelEditor.Modding"), new FSlateImageBrush(FPaths::EngineContentDir() / TEXT("Editor/Slate/Icons/icon_tab_Toolbars_40x.png"), Icon40x40));
	return Style;
}

const ISlateStyle& FJolfModEditorStyle::Get()
{
	return *StyleInstance;
}
