// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfLocalPlayer.h"
#include "Components/Widget.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "JolfPerPlayerSettings.h"
#include "JolfUserSettings.h"
#include "GameFramework/GameModeBase.h"

// Public Functions:

TSharedPtr<SWidget> UJolfLocalPlayer::GetSlateUserFocus() const
{
	TSharedPtr<const FSlateUser> SlateUser = GetSlateUser();
	return SlateUser ? SlateUser->GetFocusedWidget() : nullptr;
}

void UJolfLocalPlayer::SetSlateUserFocus(UWidget* InWidget)
{
	if (InWidget == nullptr)
		return;

	TSharedPtr<SWidget> CachedWidget = InWidget->GetCachedWidget();
	if (CachedWidget == nullptr)
		return;

	if (FSlateApplication::Get().SetUserFocus(GetControllerId(), CachedWidget))
	{
		GetSlateOperations().CancelFocusRequest();
	}
	else
	{
		GetSlateOperations().SetUserFocus(CachedWidget.ToSharedRef());
	}
}

void UJolfLocalPlayer::SetSlateUserFocus(TSharedPtr<SWidget> InWidget)
{
	if (!InWidget.IsValid())
		return;

	if (FSlateApplication::Get().SetUserFocus(GetControllerId(), InWidget))
	{
		GetSlateOperations().CancelFocusRequest();
	}
	else
	{
		GetSlateOperations().SetUserFocus(InWidget.ToSharedRef());
	}
}	

void UJolfLocalPlayer::MoveCursorToViewportCenter()
{
	if (ViewportClient)
	{
		if (FViewport* Viewport = ViewportClient->Viewport)
		{
			FIntPoint SizeXY = Viewport->GetSizeXY();
			int32 X = (Origin.X + Size.X * 0.5f) * SizeXY.X;
			int32 Y = (Origin.Y + Size.Y * 0.5f) * SizeXY.Y;
			Viewport->SetMouse(X, Y);
		}
	}
}

UJolfPerPlayerSettings* UJolfLocalPlayer::GetPlayerSettings() const
{
	check(PlayerSettings);
	checkSlow(!PlayerSettings->IsPendingKill());
	return PlayerSettings;
}

void UJolfLocalPlayer::SetPlayerSettings(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	check(!InPlayerSettings->IsPendingKill());
	check(PlayerSettings);

	if (PlayerSettings == InPlayerSettings)
		return;

	PlayerSettings->RemoveUser(this);
	PlayerSettings->OnPlayerNameChanged.Remove(NameChangedHandle);

	PlayerSettings = InPlayerSettings;
	
	PlayerSettings->AddUser(this);
	NameChangedHandle = PlayerSettings->OnPlayerNameChanged.AddUObject(this, &UJolfLocalPlayer::OnPlayerNameChanged);

	UJolfUserSettings* SystemSettings = UJolfUserSettings::GetJolfUserSettings();
	SystemSettings->SetPlayerSettingsAssociation(GetControllerId(), PlayerSettings);
	OnPlayerSettingsChanged.Broadcast(InPlayerSettings);
}

// Public Interfaces:

//~ Begin ULocalPlayer Interface
void UJolfLocalPlayer::PlayerAdded(class UGameViewportClient* InViewportClient, int32 InControllerID)
{
	Super::PlayerAdded(InViewportClient, InControllerID);

	check(PlayerSettings == nullptr);
	UJolfUserSettings* SystemSettings = UJolfUserSettings::GetJolfUserSettings();
	PlayerSettings = SystemSettings->FindOrCreatePlayerSettings(InControllerID);
	PlayerSettings->AddUser(this);
	NameChangedHandle = PlayerSettings->OnPlayerNameChanged.AddUObject(this, &UJolfLocalPlayer::OnPlayerNameChanged);
}

void UJolfLocalPlayer::PlayerRemoved()
{
	Super::PlayerRemoved();

	check(PlayerSettings);
	PlayerSettings->RemoveUser(this);
}

FString UJolfLocalPlayer::GetNickname() const
{
	// Only use settings name as player name for local multiplayer.
	UGameInstance* GameInstance = GetGameInstance();
	if (ensure(GameInstance) && GameInstance->GetNumLocalPlayers() > 1)
	{
		check(PlayerSettings);
		if (!PlayerSettings->GetPlayerName().IsEmpty())
		{
			return PlayerSettings->GetPlayerName();
		}
	}

	return Super::GetNickname();
}
//~ End ULocalPlayer Interface

// Private Functions:

void UJolfLocalPlayer::OnPlayerNameChanged(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings == PlayerSettings);
	UWorld* World = GetWorld();
	check(World);
	if (AGameModeBase* GameMode = World->GetAuthGameMode())
	{
		if (APlayerController* PC = GetPlayerController(World))
		{
			GameMode->ChangeName(PC, InPlayerSettings->GetPlayerName(), true);
		}
	}
}