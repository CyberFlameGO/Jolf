// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfGameInstance.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "JolfPawn.h"

DECLARE_LOG_CATEGORY_CLASS(LogJolfMusic, Warning, All)

#if !NO_LOGGING
static bool bLogMaxLocalPlayerSpeed = false;
static FAutoConsoleVariableRef CVarLogMaxLocalPlayerSpeed(TEXT("j.LogMaxLocalPlayerSpeed"), bLogMaxLocalPlayerSpeed, TEXT(""));
static bool bLogMusicShuffle = false;
static FAutoConsoleVariableRef CVarLogMusicShuffle(TEXT("j.LogMusicShuffle"), bLogMusicShuffle, TEXT(""));

void LogMusicPaths(const TArray<TSoftObjectPtr<USoundBase>>& InMusicPaths)
{
	if (bLogMusicShuffle)
	{
		UE_LOG(LogJolfMusic, Log, TEXT("%i music paths:"), InMusicPaths.Num());
		for (int32 Index = 0; Index < InMusicPaths.Num(); ++Index)
		{
			UE_LOG(LogJolfMusic, Log, TEXT("[%i] %s"), Index, *InMusicPaths[Index].ToString());
		}
	}
}
#endif // !NO_LOGGING

// Public Interfaces:

//~ Begin UGameInstance Interface
void UJolfGameInstance::OnStart()
{
	Super::OnStart();

	ShuffledMusicSounds = MusicSounds;
	ShuffledMusicIndex = INDEX_NONE;
	AsyncLoadNextMusic();

	SpeedAudioComponent = UGameplayStatics::SpawnSound2D(this, SpeedSound, 0.f, 1.f, 0.f, nullptr, true, false);
	if (SpeedAudioComponent) // Null in mod project.
	{
		SpeedAudioComponent->bIsVirtualized = true; // Otherwise loop restarts every time volume reaches zero which sounds bad.
	}
}
//~ End UGameInstance Interface

//~ Begin FTickableGameObject Interface
void UJolfGameInstance::Tick(float DeltaTime)
{
	float MaxSpeedSquared = 0.f;
	for (ULocalPlayer* LP : GetLocalPlayers())
	{
		APlayerController* PC = LP->PlayerController;
		if (PC == nullptr)
			continue;

		AActor* ViewTarget = PC->GetViewTarget();
		if (ViewTarget == nullptr)
			continue;

		if (!ViewTarget->IsA(AJolfPawn::StaticClass()))
		{
			// Hack to disable speed audio for spectator pawn.
			continue;
		}

		MaxSpeedSquared = FMath::Max(MaxSpeedSquared, ViewTarget->GetVelocity().SizeSquared());
	}

	float MaxSpeed = FMath::Sqrt(MaxSpeedSquared);

#if !NO_LOGGING
	UE_CLOG(bLogMaxLocalPlayerSpeed, LogTemp, Display, TEXT("MaxSpeed: %f"), MaxSpeed);
#endif // !NO_LOGGING

	if (SpeedAudioComponent) // Null in mod project.
	{
		float TargetVolume = SpeedVolumeCurve.GetRichCurve()->Eval(MaxSpeed);
		float CurrentVolume = SpeedAudioComponent->VolumeMultiplier;
		if (TargetVolume > CurrentVolume)
		{
			CurrentVolume = FMath::FInterpTo(CurrentVolume, TargetVolume, DeltaTime, SpeedVolumeRiseRate);
		}
		else
		{
			CurrentVolume = TargetVolume;
		}
		SpeedAudioComponent->SetVolumeMultiplier(CurrentVolume);
	}
}

ETickableTickType UJolfGameInstance::GetTickableTickType() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? ETickableTickType::Never : ETickableTickType::Always;
}

TStatId UJolfGameInstance::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UJolfGameInstance, STATGROUP_Tickables);
}

UWorld* UJolfGameInstance::GetTickableGameObjectWorld() const
{
	return GetWorld();
}
//~ End FTickableGameObject Interface

// Private Functions:

void UJolfGameInstance::ShuffleMusicPaths()
{
	for (int32 DestinationIndex = ShuffledMusicSounds.Num() - 1; DestinationIndex > 0; --DestinationIndex)
	{
		int32 SourceIndex = FMath::RandRange(0, DestinationIndex); // Inclusive
		if (SourceIndex != DestinationIndex)
		{
			Swap(ShuffledMusicSounds[SourceIndex], ShuffledMusicSounds[DestinationIndex]);
		}
	}

#if !NO_LOGGING
	LogMusicPaths(ShuffledMusicSounds);
#endif // !NO_LOGGING
}

void UJolfGameInstance::ReshuffleMusicPaths()
{
	// Special case to prevent last track from playing twice in a row.
	Swap(ShuffledMusicSounds[0], ShuffledMusicSounds[FMath::RandRange(0, ShuffledMusicSounds.Num() - 2)]);

	for (int32 DestinationIndex = ShuffledMusicSounds.Num() - 1; DestinationIndex > 1; --DestinationIndex)
	{
		int32 SourceIndex = FMath::RandRange(1, DestinationIndex); // Inclusive
		if (SourceIndex != DestinationIndex)
		{
			Swap(ShuffledMusicSounds[SourceIndex], ShuffledMusicSounds[DestinationIndex]);
		}
	}

#if !NO_LOGGING
	LogMusicPaths(ShuffledMusicSounds);
#endif // !NO_LOGGING
}

int32 UJolfGameInstance::AdvanceShuffledMusicIndex()
{
	if (ShuffledMusicIndex < 0)
	{
		// Clips have not been shuffled yet.
		ShuffleMusicPaths();
		ShuffledMusicIndex = 0;
	}
	else if (ShuffledMusicIndex >= ShuffledMusicSounds.Num())
	{
		ReshuffleMusicPaths();
		ShuffledMusicIndex = 0;
	}

	return ShuffledMusicIndex++;
}

void UJolfGameInstance::AsyncLoadNextMusic()
{
	if (ShuffledMusicSounds.Num() < 1)
		return;

	int32 Index = AdvanceShuffledMusicIndex();
	const FSoftObjectPath& Path = ShuffledMusicSounds[Index].ToSoftObjectPath();
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	StreamableManager.RequestAsyncLoad(Path, FStreamableDelegate::CreateUObject(this, &UJolfGameInstance::OnMusicLoaded, Index));
}

void UJolfGameInstance::OnMusicLoaded(int32 InIndex)
{
	const TSoftObjectPtr<USoundBase> SoftSound = ShuffledMusicSounds[InIndex];
	if (USoundBase* Sound = SoftSound.Get())
	{
		if (MusicComponent)
		{
			MusicComponent->SetSound(Sound);
		}
		else
		{
			MusicComponent = UGameplayStatics::CreateSound2D(this, Sound, 1.f, 1.f, 0.f, nullptr, /*bPersistAcrossLevelTransition*/ true, /*bAutoDestroy*/ false);
			MusicComponent->bIsMusic = true;
			MusicComponent->bIsVirtualized = true;
		}

		MusicComponent->FadeIn(5.f);

		// Sadly neither OnAudioFinished nor OnAudioPlayStateChanged work with bPersistAcrossLevelTransition as of 4.26.2 because
		// they have checks requiring a valid world. UDN posts indicate Epic was aware of this limitation in 2018, so we use timers instead.
		GetTimerManager().SetTimer(MusicTimerHandle, this, &UJolfGameInstance::OnMusicTimerFired, Sound->GetDuration(), false);
	}
	else
	{
		UE_LOG(LogJolfMusic, Warning, TEXT("Unable to load music: %s"), *SoftSound.ToString());
	}
}

void UJolfGameInstance::OnMusicTimerFired()
{
	check(MusicComponent);
	UE_LOG(LogJolfMusic, Log, TEXT("MusicTimerFired %s"), *GetNameSafe(MusicComponent->Sound));
	AsyncLoadNextMusic();
}