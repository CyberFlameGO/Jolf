// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfViewportClient.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/Engine.h"
#include "Engine/Console.h"
#include "JolfPlayerController.h"
#include "JolfLocalPlayer.h"
#include "JolfSlateFocus.h"

UJolfViewportClient::UJolfViewportClient(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HotseatIndex = INDEX_NONE;
	bHotseatMode = false;

	// Jolf hotseat uses per-player non-splitscreen layout.
	SplitscreenInfo[ESplitScreenType::None].PlayerData.Add(FPerPlayerSplitscreenData(0.f, 0.f, 0.f, 0.f));
	check(SplitscreenInfo[ESplitScreenType::None].PlayerData.Num() == 2);
}

// Public Functions:

UJolfLocalPlayer* UJolfViewportClient::GetHotseatLocalPlayer() const
{
	if (ensure(GameInstance) && GameInstance->GetLocalPlayers().IsValidIndex(HotseatIndex))
	{
		return CastChecked<UJolfLocalPlayer>(GameInstance->GetLocalPlayers()[HotseatIndex]);
	}
	else
	{
		return nullptr;
	}
}

AJolfPlayerController* UJolfViewportClient::GetHotseatPlayerController() const
{
	if (ensure(GameInstance) && GameInstance->GetLocalPlayers().IsValidIndex(HotseatIndex))
	{
		ULocalPlayer* LP = GameInstance->GetLocalPlayers()[HotseatIndex];
		check(LP);
		return Cast<AJolfPlayerController>(LP->PlayerController);
	}
	else
	{
		return nullptr;
	}
}

void UJolfViewportClient::SetHotseatIndex(int32 InLocalPlayerIndex)
{
	if (HotseatIndex == InLocalPlayerIndex)
		return;

	HotseatIndex = InLocalPlayerIndex;

	// Hotseat player (if any) gets full viewport, and other players get collapsed viewport.
	const int32 FullscreenIndex = HotseatIndex == INDEX_NONE ? 0 : HotseatIndex;
	for (int32 PlayerIndex = 0; PlayerIndex < 2; ++PlayerIndex)
	{
		FPerPlayerSplitscreenData& SplitscreenData = SplitscreenInfo[ESplitScreenType::None].PlayerData[PlayerIndex];
		if (PlayerIndex == FullscreenIndex)
		{
			SplitscreenData.SizeX = 1.f;
			SplitscreenData.SizeY = 1.f;
		}
		else
		{
			SplitscreenData.SizeX = 0.f;
			SplitscreenData.SizeY = 0.f;
		}
	}

	LayoutPlayers();
}

// Public Interfaces:

//~ Begin FViewportClient Interface
EMouseCursor::Type UJolfViewportClient::GetCursor(FViewport* InViewport, int32 X, int32 Y)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Mostly identical implementation to super, but routes GetMouseCursor for hotseat and splitscreen.
	// Removes Viewport->HasFocus() check because that uses index zero.
	////////////////////////////////////////////////////////////////////////////////////////////////////

	if (!FJolfSlateFocus::bDrawMouseHover)
	{
		// Hide mouse cursor while using gamepad navigation. 
		return EMouseCursor::None;
	}

	// If the viewport isn't active or the console is active we don't want to override the cursor
	if (!FSlateApplication::Get().IsActive())
	{
		return EMouseCursor::Default;
	}
	else if (!InViewport->HasMouseCapture())
	{
		return EMouseCursor::Default;
	}
	else if (ViewportConsole && ViewportConsole->ConsoleActive())
	{
		return EMouseCursor::Default;
	}

	int32 MouseControllerId = FSlateApplication::Get().GetUserIndexForMouse();
	ULocalPlayer* LocalPlayer = GEngine->GetLocalPlayerFromControllerId(this, MouseControllerId);
	if (LocalPlayer && LocalPlayer->PlayerController)
	{
		return LocalPlayer->PlayerController->GetMouseCursor();
	}

	return FViewportClient::GetCursor(InViewport, X, Y);
}

bool UJolfViewportClient::InputKey(const FInputKeyEventArgs& InEventArgs)
{
	FInputKeyEventArgs EventArgs = InEventArgs;
	if (HotseatIndex >= 0)
	{
		EventArgs.ControllerId = HotseatIndex;
	}

	return Super::InputKey(EventArgs);
}

bool UJolfViewportClient::InputAxis(FViewport* InViewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	if (HotseatIndex >= 0)
	{
		ControllerId = HotseatIndex;
	}

	return Super::InputAxis(InViewport, ControllerId, Key, Delta, DeltaTime, NumSamples, bGamepad);
}
//~ End FViewportClient Interface

//~ Begin UGameViewportClient Interface
void UJolfViewportClient::UpdateActiveSplitscreenType()
{
	if (IsHotseatMode())
	{
		// "None" splitscreen data will be adjusted accordingly by SetHotseatIndex.
		ActiveSplitscreenType = ESplitScreenType::None;
	}
	else
	{
		Super::UpdateActiveSplitscreenType();
	}
}
//~ End UGameViewportClient Interface