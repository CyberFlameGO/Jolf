// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/PlayerState.h"
#include "JolfPlayerState.generated.h"

USTRUCT()
struct FJolfPlayerStateScore
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 HoleIndex;

	UPROPERTY(EditAnywhere)
	int32 Strokes;

	/** Seconds from spawn to completion. */
	UPROPERTY(EditAnywhere)
	float Time;
};

UCLASS()
class JOLF_API AJolfPlayerState : public APlayerState
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UObject Interface
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin APlayerState Interface
	void OnRep_PlayerName() override;
	//~ End APlayerState Interface

public: // Functions

	class AJolfHole* GetCurrentHoleActor() const;

	const TArray<FJolfPlayerStateScore>& GetHoleScores() const { return HoleScores; }
	void SetHoleScore(int32 InHoleIndex, int32 InStrokes, float InTime);

public: // Properties

	UPROPERTY(EditAnywhere, Transient, Replicated)
	int32 StrokeIndex;

	UPROPERTY(EditAnywhere, Transient, Replicated)
	int32 HoleIndex;

	/** Number of mulligans used on the current hole. */
	UPROPERTY(EditAnywhere, Transient, Replicated)
	int32 NumMulligansUsed;

	DECLARE_EVENT_OneParam(AJolfPlayerState, FOnPlayerNameChangedEvent, AJolfPlayerState*);
	FOnPlayerNameChangedEvent OnPlayerNameChanged;

	FOnPlayerNameChangedEvent OnHoleScoresChanged;

private: // Functions

	UFUNCTION()
	void OnRep_HoleScores();

private: // Properties
	
	UPROPERTY(EditAnywhere, Transient, ReplicatedUsing = OnRep_HoleScores)
	TArray<FJolfPlayerStateScore> HoleScores;
};