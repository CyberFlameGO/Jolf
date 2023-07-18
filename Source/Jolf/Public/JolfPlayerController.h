// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/PlayerController.h"
#include "JolfPlayerController.generated.h"

UCLASS()
class JOLF_API AJolfPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AJolfPlayerController(const FObjectInitializer& ObjectInitializer);

public: // Functions

	UFUNCTION(Server, Reliable)
	void ServerRestartHole();

	UFUNCTION(Server, Reliable)
	void ServerTextChat(const FText& InText);

	UFUNCTION(Server, Reliable)
	void ClientTextChat(class AJolfPlayerState* InSpeaker, const FText& InText);

	UFUNCTION(Server, Reliable)
	void ServerSetPawnCollisionEnabled(bool bInEnablePawnCollision);

	UFUNCTION(Server, Reliable)
	void ServerSetSequentialTurnsEnabled(bool bInSequentialTurns);

	UFUNCTION(Server, Reliable)
	void ServerSetWaitForAllPlayers(bool bInWaitForAllPlayers);
	
	UFUNCTION(Server, Reliable)
	void ServerSetAllowStrokeWhileMoving(bool bInAllowStrokeWhileMoving);

	UFUNCTION(Server, Reliable)
	void ServerSetNumFreeMulligans(int32 InNumFreeMulligans);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestTravel(const FGuid& InLevelGuid);

	void BecomeHotseatActivePlayer();
	void ConfirmHotseat();
	void OnTextChatFinished();
	void ClosePauseMenu();
	void CloseScorecard();

	class UJolfPerPlayerSettings* GetPlayerSettings() const;

public: // Interfaces

	//~ Begin AActor Interface
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	//~ Begin APlayerController Interface
	void AutoManageActiveCameraTarget(AActor* SuggestedTarget) override;
	void ReceivedPlayer() override;
	void UpdateHiddenActors(const FVector& ViewLocation) override;
	void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	void SetInputMode(const FInputModeDataBase& InData) override;
	//~ End APlayerController Interface

public: // Properties

	FSimpleMulticastDelegate OnScorecardOpened;
	FSimpleMulticastDelegate OnTextChatRequested;

	DECLARE_MULTICAST_DELEGATE_TwoParams(FTextChatDelegate, class AJolfPlayerState*, const FText&);
	FTextChatDelegate OnTextChatReceived;

	UPROPERTY()
	uint8 bWantsOverlay:1;

protected: // Interfaces

	//~ Begin APlayerController Interface
	void SetupInputComponent() override;
	//~ End APlayerController Interface

private: // Functions

	void OnMenuPressed();
	void OnScorecardPressed();
	void OnFreecamPressed();
	void OnOverlayPressed();
	void OnOverlayReleased();
	void OnViewNextPressed();
	void OnViewPrevPressed();
	void OnTextChatPressed();

	UFUNCTION(Server, Unreliable)
	void ServerViewNextPlayer_Jolf();
	UFUNCTION(Server, Unreliable)
	void ServerViewPrevPlayer_Jolf();

	void ApplyCustomInputMode();

private: // Properties

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> PauseMenuClass;

	UPROPERTY()
	class UUserWidget* PauseMenuInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> ScorecardClass;

	UPROPERTY()
	class UUserWidget* ScorecardInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> HotseatConfirmationClass;

	UPROPERTY()
	class UUserWidget* HotseatConfirmationInstance;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> ChatClass;

	UPROPERTY()
	class UUserWidget* ChatInstance;
	
	UPROPERTY(Transient)
	uint8 bWaitingForHotseatConfirmation:1;

	UPROPERTY(Transient)
	uint8 bPauseMenuActive:1;
	
	UPROPERTY(Transient)
	uint8 bScorecardActive:1;
};