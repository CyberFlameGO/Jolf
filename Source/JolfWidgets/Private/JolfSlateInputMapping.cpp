// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfSlateInputMapping.h"
#include "JolfViewportClient.h"
#include "Engine/Engine.h"
#include "JolfUserSettings.h"

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
static int32 OverrideUserIndexForMouse = INDEX_NONE; // Useful for debugging slate input issues.
static FAutoConsoleVariableRef CVarOverrideUserIndexForMouse(TEXT("j.OverrideUserIndexForMouse"), OverrideUserIndexForMouse, TEXT(""));
static int32 OverrideUserIndexForKeyboard = INDEX_NONE; // Useful for debugging slate input issues.
static FAutoConsoleVariableRef CVarOverrideUserIndexForKeyboard(TEXT("j.OverrideUserIndexForKeyboard"), OverrideUserIndexForKeyboard, TEXT(""));
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

//~ Begin ISlateInputManager Interface
int32 FJolfSlateInputMapping::GetUserIndexForMouse() const
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (OverrideUserIndexForMouse != INDEX_NONE)
		return OverrideUserIndexForMouse;
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	// Unfortunately unreal still makes a variety of assumptions about the mouse user being index zero.
	//
	//if (UJolfViewportClient* Viewport = Cast<UJolfViewportClient>(GEngine->GameViewport))
	//{
	//	if (Viewport->GetHotseatIndex() != INDEX_NONE)
	//	{
	//		return Viewport->GetHotseatIndex();
	//	}
	//}
	
	return 0;
}

int32 FJolfSlateInputMapping::GetUserIndexForKeyboard() const
{
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (OverrideUserIndexForKeyboard != INDEX_NONE)
		return OverrideUserIndexForKeyboard;
#endif // !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

	if (UJolfViewportClient* Viewport = Cast<UJolfViewportClient>(GEngine->GameViewport))
	{
		if (Viewport->GetHotseatIndex() != INDEX_NONE)
		{
			return Viewport->GetHotseatIndex();
		}
	}

	return 0;
}

int32 FJolfSlateInputMapping::GetUserIndexForController(int32 ControllerId) const
{
	if (UJolfViewportClient* Viewport = Cast<UJolfViewportClient>(GEngine->GameViewport))
	{
		if (Viewport->GetHotseatIndex() != INDEX_NONE)
		{
			return Viewport->GetHotseatIndex();
		}

		if (Viewport->GetCurrentSplitscreenConfiguration() != ESplitScreenType::None)
		{
			const UJolfUserSettings* Settings = UJolfUserSettings::GetJolfUserSettings();
			return ControllerId + Settings->bOffsetGamepadUserIndex;
		}
	}

	// While not in splitscreen or hotseat allow gamepad to be used as primary input.
	return 0;
}

TOptional<int32> FJolfSlateInputMapping::GetUserIndexForController(int32 ControllerId, FKey InKey) const
{
	return GetUserIndexForController(ControllerId);
}
//~ End ISlateInputManager Interface