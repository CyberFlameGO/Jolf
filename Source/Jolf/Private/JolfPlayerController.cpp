// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "JolfPlayerState.h"
#include "GameFramework/GameModeBase.h"
#include "JolfSpectatorPawn.h"
#include "JolfCheatManager.h"
#include "JolfPawn.h"
#include "EngineUtils.h"
#include "JolfViewportClient.h"
#include "Widgets/SViewport.h"
#include "JolfLocalPlayer.h"
#include "Framework/Application/SlateApplication.h"
#include "JolfSceneProbeSubsystem.h"
#include "JolfLevelDefinition.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "JolfGameMode.h"
#include "JolfGameState.h"

AJolfPlayerController::AJolfPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CheatClass = UJolfCheatManager::StaticClass();
}

// Public Functions:

void AJolfPlayerController::ServerRestartHole_Implementation()
{
	AJolfPlayerState* PS = GetPlayerState<AJolfPlayerState>();
	if (!ensure(PS))
		return;

	PS->StrokeIndex = 0;
	PS->NumMulligansUsed = 0;

	StartSpot.Reset();
	if (APawn* PreRestartPawn = GetPawn())
	{
		PreRestartPawn->Destroy();
	}
	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

void AJolfPlayerController::ServerTextChat_Implementation(const FText& InText)
{
	if (InText.IsEmptyOrWhitespace())
		return;

	AJolfPlayerState* Speaker = GetPlayerState<AJolfPlayerState>();
	if (Speaker == nullptr)
		return;

#if WITH_EDITOR
	// Todo: only admins should be able to use exec.
	const FString& MessageString = InText.ToString();
	if (MessageString.StartsWith(TEXT("/")))
	{
		if (MessageString.Len() > 1)
		{
			const FString Command = MessageString.RightChop(1);
			const bool bWriteToLog = false; // Return output rather than logging.
			const FString Output = ConsoleCommand(Command, bWriteToLog);
			ClientTextChat(Speaker, FText::AsCultureInvariant(Output));
		}
		else
		{
			ClientTextChat(Speaker, INVTEXT("Empty command"));
		}
		return;
	}
#endif // WITH_EDITOR

	for (auto Iter = GetWorld()->GetPlayerControllerIterator(); Iter; ++Iter)
	{
		AJolfPlayerController* PC = Cast<AJolfPlayerController>(*Iter);
		if (PC == nullptr)
			continue;

		PC->ClientTextChat(Speaker, InText);
	}
}

void AJolfPlayerController::ClientTextChat_Implementation(AJolfPlayerState* InSpeaker, const FText& InText)
{
	OnTextChatReceived.Broadcast(InSpeaker, InText);
}

void AJolfPlayerController::ServerSetPawnCollisionEnabled_Implementation(bool bInEnablePawnCollision)
{
	if (AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>())
	{
		GameMode->SetEnablePawnCollision(bInEnablePawnCollision);
	}
}

void AJolfPlayerController::ServerSetSequentialTurnsEnabled_Implementation(bool bInSequentialTurns)
{
	if (AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>())
	{
		GameMode->SetEnableSequentialTurns(bInSequentialTurns);
	}
}

void AJolfPlayerController::ServerSetWaitForAllPlayers_Implementation(bool bInWaitForAllPlayers)
{
	if (AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>())
	{
		GameMode->SetWaitForAllPlayers(bInWaitForAllPlayers);
	}
}

void AJolfPlayerController::ServerSetAllowStrokeWhileMoving_Implementation(bool bInAllowStrokeWhileMoving)
{
	if (AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>())
	{
		GameMode->SetAllowStrokeWhileMoving(bInAllowStrokeWhileMoving);
	}
}

void AJolfPlayerController::ServerSetNumFreeMulligans_Implementation(int32 InNumFreeMulligans)
{
	if (AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
	{
		GameState->NumFreeMulligans = InNumFreeMulligans;
	}
}

bool AJolfPlayerController::ServerRequestTravel_Validate(const FGuid& InLevelGuid)
{
	RPC_VALIDATE(InLevelGuid.IsValid())
	return true;
}

void AJolfPlayerController::ServerRequestTravel_Implementation(const FGuid& InLevelGuid)
{
	UJolfLevelDefinition* LevelDef = UJolfLevelDefinition::FindByGuid(InLevelGuid);
	if (LevelDef == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server cannot find level definition for requested guid %s"), *InLevelGuid.ToString());
		return;
	}

	if (AJolfGameMode* GameMode = GetWorld()->GetAuthGameMode<AJolfGameMode>())
	{
		GameMode->RequestTravel(*LevelDef);
	}
}

void AJolfPlayerController::BecomeHotseatActivePlayer()
{
	if (ensure(HotseatConfirmationInstance))
	{
		bWaitingForHotseatConfirmation = true;
		HotseatConfirmationInstance->SetVisibility(ESlateVisibility::Visible);
		ApplyCustomInputMode();
	}
}

void AJolfPlayerController::ConfirmHotseat()
{
	bWaitingForHotseatConfirmation = false;
	ApplyCustomInputMode();
}

void AJolfPlayerController::OnTextChatFinished()
{
	ApplyCustomInputMode();
}

void AJolfPlayerController::ClosePauseMenu()
{
	// todo
	if (bPauseMenuActive)
	{
		OnMenuPressed();
	}
}

void AJolfPlayerController::CloseScorecard()
{
	// todo
	if (bScorecardActive)
	{
		OnScorecardPressed();
	}
}

class UJolfPerPlayerSettings* AJolfPlayerController::GetPlayerSettings() const
{
	return CastChecked<UJolfLocalPlayer>(Player)->GetPlayerSettings();
}

// Public Interfaces:

//~ Begin AActor Interface
void AJolfPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (PauseMenuInstance)
	{
		PauseMenuInstance->RemoveFromViewport();
		PauseMenuInstance = nullptr;
	}

	if (ScorecardInstance)
	{
		ScorecardInstance->RemoveFromViewport();
		ScorecardInstance = nullptr;
	}

	if (ChatInstance)
	{
		ChatInstance->RemoveFromViewport();
		ChatInstance = nullptr;
	}

	if (HotseatConfirmationInstance)
	{
		HotseatConfirmationInstance->RemoveFromViewport();
		HotseatConfirmationInstance = nullptr;
	}
}
//~ End AActor Interface

//~ Begin APlayerController Interface
void AJolfPlayerController::AutoManageActiveCameraTarget(AActor* SuggestedTarget)
{
	// Active player in sequential turns mode takes priority.
	if (bAutoManageActiveCameraTarget)
	{
		if (AJolfGameState* GameState = GetWorld()->GetGameState<AJolfGameState>())
		{
			if (GameState->bSequentialTurns && GameState->ActivePlayerState)
			{
				SetViewTarget(GameState->ActivePlayerState);
				return;
			}
		}
	}

	Super::AutoManageActiveCameraTarget(SuggestedTarget);
}

void AJolfPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalPlayerController())
	{
		PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
		if (ensure(PauseMenuInstance))
		{
			PauseMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
			PauseMenuInstance->AddToPlayerScreen(300);
		}

		ScorecardInstance = CreateWidget<UUserWidget>(this, ScorecardClass);
		if (ensure(ScorecardInstance))
		{
			ScorecardInstance->SetVisibility(ESlateVisibility::Collapsed);
			ScorecardInstance->AddToPlayerScreen(200);
		}

		HotseatConfirmationInstance = CreateWidget<UUserWidget>(this, HotseatConfirmationClass);
		if (ensure(HotseatConfirmationInstance))
		{
			HotseatConfirmationInstance->SetVisibility(ESlateVisibility::Collapsed);
			HotseatConfirmationInstance->AddToPlayerScreen(500);
		}

		if (UJolfViewportClient* Viewport = CastChecked<UJolfViewportClient>(GetLocalPlayer()->ViewportClient))
		{
			if (Viewport->IsHotseatMode())
			{
				bWaitingForHotseatConfirmation = true;
			}
		}

		ChatInstance = CreateWidget<UUserWidget>(this, ChatClass);
		if (ensure(ChatInstance))
		{
			ChatInstance->AddToPlayerScreen(100);
		}

		ApplyCustomInputMode();
	}
}

void AJolfPlayerController::UpdateHiddenActors(const FVector& ViewLocation)
{
	HiddenActors.Reset();

	Super::UpdateHiddenActors(ViewLocation);
	
	// If controlled pawn has not moved yet then hide every other pawn which has not moved yet. Otherwise start is cluttered.
	const AJolfPawn* JolfPawn = GetPawn<AJolfPawn>();
	if (JolfPawn && !JolfPawn->HasMovedYet())
	{
		for (TActorIterator<AJolfPawn> PawnIter(GetWorld()); PawnIter; ++PawnIter)
		{
			if (*PawnIter != JolfPawn && !PawnIter->HasMovedYet())
			{
				HiddenActors.Add(*PawnIter);
			}
		}
	}
}

void AJolfPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
	Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);
	
	if (UJolfSceneProbeSubsystem* SceneProbeSubsystem = GetWorld()->GetSubsystem<UJolfSceneProbeSubsystem>())
	{
		SceneProbeSubsystem->UpdateHiddenComponents(OutHiddenComponents);
	}
}

void AJolfPlayerController::SetInputMode(const FInputModeDataBase& InData)
{
	// Overridden to catch usage, and notice if the API changes in a future engine release.
	// Replaced by ApplyCustomInputMode.
	checkNoEntry();
	Super::SetInputMode(InData);	
}
//~ End APlayerController Interface

// Protected Interfaces:

//~ Begin APlayerController Interface
void AJolfPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction(TEXT("Menu"), EInputEvent::IE_Pressed, this, &AJolfPlayerController::OnMenuPressed);
	InputComponent->BindAction(TEXT("Scorecard"), EInputEvent::IE_Pressed, this, &AJolfPlayerController::OnScorecardPressed);
	InputComponent->BindAction(TEXT("Freecam"), EInputEvent::IE_Pressed, this, &AJolfPlayerController::OnFreecamPressed);
	InputComponent->BindAction(TEXT("Overlay"), EInputEvent::IE_Pressed, this, &AJolfPlayerController::OnOverlayPressed);
	InputComponent->BindAction(TEXT("Overlay"), EInputEvent::IE_Released, this, &AJolfPlayerController::OnOverlayReleased);
	InputComponent->BindAction(TEXT("ViewNext"), EInputEvent::IE_Pressed, this, &AJolfPlayerController::OnViewNextPressed);
	InputComponent->BindAction(TEXT("ViewPrev"), EInputEvent::IE_Released, this, &AJolfPlayerController::OnViewPrevPressed);

	// Pressed rather than released because otherwise the release after commit would re-open chat.
	InputComponent->BindAction(TEXT("TextChat"), EInputEvent::IE_Pressed, this, &AJolfPlayerController::OnTextChatPressed);
}
//~ End APlayerController Interface

// Private Functions:

void AJolfPlayerController::OnMenuPressed()
{
	if (PauseMenuInstance->GetVisibility() == ESlateVisibility::Collapsed)
	{
		PauseMenuInstance->SetVisibility(ESlateVisibility::Visible);
		bPauseMenuActive = true;
	}
	else
	{
		PauseMenuInstance->SetVisibility(ESlateVisibility::Collapsed);
		bPauseMenuActive = false;
	}
	ApplyCustomInputMode();
}

void AJolfPlayerController::OnScorecardPressed()
{
	if (ScorecardInstance->GetVisibility() == ESlateVisibility::Collapsed)
	{
		ScorecardInstance->SetVisibility(ESlateVisibility::Visible);
		bScorecardActive = true;
		OnScorecardOpened.Broadcast();
	}
	else
	{
		ScorecardInstance->SetVisibility(ESlateVisibility::Collapsed);
		bScorecardActive = false;
	}
	ApplyCustomInputMode();
}

void AJolfPlayerController::OnFreecamPressed()
{
	APawn* CurrentPawn = GetPawn();
	if (AJolfSpectatorPawn* CurrentFreecamPawn = Cast<AJolfSpectatorPawn>(CurrentPawn))
	{
		APawn* OriginalPawn = CurrentPawn->GetInstigator();
		if (ensure(OriginalPawn))
		{
			FRotator CurrentControlRotation = GetControlRotation();

			Possess(OriginalPawn);
			CurrentFreecamPawn->Destroy();

			// Otherwise possess overrides with pawn rotation.
			SetControlRotation(CurrentControlRotation);
		}
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = CurrentPawn;
		SpawnParams.Owner = this;
		SpawnParams.ObjectFlags |= RF_Transient;

		FVector ViewLocation;
		FRotator ViewRotation;
		GetPlayerViewPoint(ViewLocation, ViewRotation);
		FTransform SpawnTransform(ViewRotation, ViewLocation);

		AJolfSpectatorPawn* Spectator = GetWorld()->SpawnActor<AJolfSpectatorPawn>(AJolfSpectatorPawn::StaticClass(), SpawnTransform, SpawnParams);
		if (ensure(Spectator))
		{
			Possess(Spectator);
		}
	}
}

void AJolfPlayerController::OnOverlayPressed()
{
	bWantsOverlay = true;
}

void AJolfPlayerController::OnOverlayReleased()
{
	bWantsOverlay = false;
}

void AJolfPlayerController::OnViewNextPressed()
{
	ServerViewNextPlayer_Jolf();
}

void AJolfPlayerController::OnViewPrevPressed()
{
	ServerViewPrevPlayer_Jolf();
}

void AJolfPlayerController::OnTextChatPressed()
{
	OnTextChatRequested.Broadcast();
}

void AJolfPlayerController::ServerViewNextPlayer_Jolf_Implementation()
{
	AJolfPawn* CurrentPawn = GetPawn<AJolfPawn>();
	if (CurrentPawn && CurrentPawn->bWantsToFinishHole)
	{
		ViewAPlayer(+1);
	}
}

void AJolfPlayerController::ServerViewPrevPlayer_Jolf_Implementation()
{
	AJolfPawn* CurrentPawn = GetPawn<AJolfPawn>();
	if (CurrentPawn && CurrentPawn->bWantsToFinishHole)
	{
		ViewAPlayer(-1);
	}
}

void AJolfPlayerController::ApplyCustomInputMode()
{
	UJolfLocalPlayer* LocalPlayer = CastChecked<UJolfLocalPlayer>(Player);
	check(LocalPlayer);
	UJolfViewportClient* ViewportClient = CastChecked<UJolfViewportClient>(LocalPlayer->ViewportClient);
	check(ViewportClient);
	TSharedPtr<SViewport> ViewportWidgetPtr = ViewportClient->GetGameViewportWidget();
	if (!ViewportWidgetPtr.IsValid())
		return;
	TSharedRef<SViewport> ViewportWidgetRef = ViewportWidgetPtr.ToSharedRef();

	FReply& SlateOperations = LocalPlayer->GetSlateOperations();

	bool bWasMouseVisible = bShowMouseCursor;

	UWidget* WidgetToFocus = nullptr;
	if (bWaitingForHotseatConfirmation)
	{
		bShowMouseCursor = false;
		WidgetToFocus = HotseatConfirmationInstance;
	}
	else if (bPauseMenuActive)
	{
		bShowMouseCursor = true;
		WidgetToFocus = PauseMenuInstance;
	}
	else if (bScorecardActive)
	{
		bShowMouseCursor = true;
		WidgetToFocus = ScorecardInstance;
	}
	else
	{
		bShowMouseCursor = false;
	}

	const bool bIsMouseUser = ViewportClient->IsHotseatMode() || LocalPlayer->GetControllerId() == 0;
	if (bIsMouseUser)
	{
		if (WidgetToFocus)
		{
			SlateOperations.ReleaseMouseLock();
			SlateOperations.ReleaseMouseCapture(); // Also resets UseHighPrecisionMouseMovement.

			ViewportClient->SetHideCursorDuringCapture(false);
			ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
			ViewportClient->SetMouseLockMode(EMouseLockMode::DoNotLock);

			// Not ideal, but calling release pointer capture seems to work well for ensuring mouse is actually released.
			FSlateApplication::Get().ReleaseAllPointerCapture();
		}
		else
		{
			SlateOperations.UseHighPrecisionMouseMovement(ViewportWidgetRef);
			SlateOperations.SetUserFocus(ViewportWidgetRef);
			SlateOperations.LockMouseToWidget(ViewportWidgetRef);
			SlateOperations.CaptureMouse(ViewportWidgetRef);

			ViewportClient->SetHideCursorDuringCapture(true);
			ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
			ViewportClient->SetMouseLockMode(EMouseLockMode::LockAlways);
		}

		if (bShowMouseCursor && !bWasMouseVisible)
		{
			LocalPlayer->MoveCursorToViewportCenter();
		}
	}

	if (WidgetToFocus)
	{
		LocalPlayer->SetSlateUserFocus(WidgetToFocus);
	}
}