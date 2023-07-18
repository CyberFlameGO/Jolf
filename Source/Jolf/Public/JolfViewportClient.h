// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Engine/GameViewportClient.h"
#include "JolfViewportClient.generated.h"

UCLASS()
class JOLF_API UJolfViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:

	UJolfViewportClient(const FObjectInitializer& ObjectInitializer);

public: // Functions

	class UJolfLocalPlayer* GetHotseatLocalPlayer() const;
	class AJolfPlayerController* GetHotseatPlayerController() const;

	int32 GetHotseatIndex() const { return HotseatIndex; }
	void SetHotseatIndex(int32 InLocalPlayerIndex);
	bool IsHotseatMode() const { return bHotseatMode; }

public: // Interfaces

	//~ Begin FViewportClient Interface
	EMouseCursor::Type GetCursor(FViewport* Viewport, int32 X, int32 Y) override;
	bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	bool InputAxis(FViewport* InViewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) override;
	//~ End FViewportClient Interface

	//~ Begin UGameViewportClient Interface
	void UpdateActiveSplitscreenType() override;
	//~ End UGameViewportClient Interface

public: // Properties

	UPROPERTY(VisibleInstanceOnly, Category = "Jolf")
	uint8 bHotseatMode:1;

private: // Properties

	/** Index of local player with viewport focus, or INDEX_NONE while not in hotseat mode. */
	UPROPERTY(VisibleInstanceOnly, Category = "Jolf")
	int32 HotseatIndex;
};