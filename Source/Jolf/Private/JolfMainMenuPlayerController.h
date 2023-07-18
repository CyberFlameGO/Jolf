// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/PlayerController.h"
#include "JolfMainMenuPlayerController.generated.h"

UCLASS()
class JOLF_API AJolfMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin AActor Interface
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Interface

	//~ Begin APlayerController Interface
	void ReceivedPlayer() override;
	//~ End APlayerController Interface

private: // Properties

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY()
	UUserWidget* WidgetInstance;
};