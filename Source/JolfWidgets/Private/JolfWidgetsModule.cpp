// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Modules/ModuleManager.h"
#include "JolfSlateInputMapping.h"
#include "Framework/Application/SlateApplication.h"

class FJolfWidgetsModule : public FDefaultGameModuleImpl
{
	static TSharedPtr<FJolfSlateInputMapping> InputMappingPtr;
	static TSharedPtr<FJolfSlateNavigationConfig> NavigationConfigPtr;

	void StartupModule() override
	{
		if (FSlateApplication::IsInitialized())
		{
			TSharedRef<FJolfSlateInputMapping> InputMappingRef = MakeShared<FJolfSlateInputMapping>();
			FSlateApplication::Get().SetInputManager(InputMappingRef);
			InputMappingPtr = InputMappingRef;

			TSharedRef< FJolfSlateNavigationConfig> NavigationConfigRef = MakeShared< FJolfSlateNavigationConfig>();
			FSlateApplication::Get().SetNavigationConfig(NavigationConfigRef);
			NavigationConfigPtr = NavigationConfigRef;
		}
	}
};

TSharedPtr<FJolfSlateInputMapping> FJolfWidgetsModule::InputMappingPtr = nullptr;
TSharedPtr<FJolfSlateNavigationConfig> FJolfWidgetsModule::NavigationConfigPtr = nullptr;

IMPLEMENT_GAME_MODULE(FJolfWidgetsModule, JolfWidgets);
