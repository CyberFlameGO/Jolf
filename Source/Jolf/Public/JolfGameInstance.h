// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Engine/GameInstance.h"
#include "Tickable.h"
#include "JolfGameInstance.generated.h"

UCLASS()
class JOLF_API UJolfGameInstance : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UGameInstance Interface
	void OnStart() override;
	//~ End UGameInstance Interface

	//~ Begin FTickableGameObject Interface
	void Tick(float DeltaTime) override;
	ETickableTickType GetTickableTickType() const override;
	TStatId GetStatId() const override;
	UWorld* GetTickableGameObjectWorld() const;
	//~ End FTickableGameObject Interface

protected: // Properties

	UPROPERTY(EditAnywhere)
	class USoundBase* SpeedSound;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve SpeedVolumeCurve;

	UPROPERTY(EditAnywhere)
	float SpeedVolumeRiseRate;

	UPROPERTY(EditAnywhere)
	TArray<TSoftObjectPtr<class USoundBase>> MusicSounds;

private: // Functions

	friend class AJolfMusicShuffleTest;

	/**
	 * Durstenfeld version of Fisher-Yates shuffle:
	 * https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm
	 */
	void ShuffleMusicPaths();
	/** Same as above, but prevent the last clip from being shuffled to the front in order to prevent repeats. */
	void ReshuffleMusicPaths();

	int32 AdvanceShuffledMusicIndex();
	void AsyncLoadNextMusic();
	void OnMusicLoaded(int32 InIndex);
	void OnMusicTimerFired();
	
private: // Properties

	UPROPERTY(Transient)
	class UAudioComponent* MusicComponent;

	/**
	 * Plays fast wind sounds based on the speed of local player view targets.
	 * Originally implemented in pawns, but that was problematic with multiple local players moving simultaneously.
	 */
	UPROPERTY(Transient)
	class UAudioComponent* SpeedAudioComponent;

	UPROPERTY(Transient)
	TArray<TSoftObjectPtr<class USoundBase>> ShuffledMusicSounds;

	UPROPERTY(Transient)
	int32 ShuffledMusicIndex;

	UPROPERTY()
	FTimerHandle MusicTimerHandle;
};