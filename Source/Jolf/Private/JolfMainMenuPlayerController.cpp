// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfMainMenuPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "JolfLocalPlayer.h"
#include "JolfViewportClient.h"
#include "Widgets/SViewport.h"

// Public Interfaces:

//~ Begin AActor Interface
void AJolfMainMenuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (WidgetInstance)
	{
		WidgetInstance->RemoveFromViewport();
		WidgetInstance = nullptr;
	}
}
//~ End AActor Interface

//~ Begin APlayerController Interface
void AJolfMainMenuPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	UJolfLocalPlayer* LocalPlayer = CastChecked<UJolfLocalPlayer>(Player);
	check(LocalPlayer);
	UJolfViewportClient* ViewportClient = CastChecked<UJolfViewportClient>(LocalPlayer->ViewportClient);
	check(ViewportClient);
	TSharedPtr<SViewport> ViewportWidgetPtr = ViewportClient->GetGameViewportWidget();
	if (!ViewportWidgetPtr.IsValid())
		return;
	TSharedRef<SViewport> ViewportWidgetRef = ViewportWidgetPtr.ToSharedRef();

	FReply& SlateOperations = LocalPlayer->GetSlateOperations();
	SlateOperations.ReleaseMouseLock();
	SlateOperations.ReleaseMouseCapture(); // Also resets UseHighPrecisionMouseMovement.

	ViewportClient->SetHideCursorDuringCapture(false);
	ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
	ViewportClient->SetMouseLockMode(EMouseLockMode::DoNotLock);

	bShowMouseCursor = true;

	WidgetInstance = CreateWidget<UUserWidget>(this, WidgetClass);
	if (ensure(WidgetInstance))
	{
		WidgetInstance->AddToPlayerScreen(300);
		LocalPlayer->SetSlateUserFocus(WidgetInstance);
	}
}
//~ End APlayerController Interface