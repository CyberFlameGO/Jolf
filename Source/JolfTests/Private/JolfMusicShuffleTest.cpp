// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfMusicShuffleTest.h"
#include "JolfGameInstance.h"

AJolfMusicShuffleTest::AJolfMusicShuffleTest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Author = TEXT("Nelson");
	Description = TEXT("Shuffle should not repeat the same song twice in a row.");
}

// Protected Interfaces:

//~ Begin AFunctionalTest Interface
void AJolfMusicShuffleTest::StartTest()
{
	Super::StartTest();

	UJolfGameInstance* GameInstance = GetWorld()->GetGameInstance<UJolfGameInstance>();
	if (AssertIsValid(GetGameInstance(), TEXT("game instance")))
	{
		GameInstance->ShuffleMusicPaths();

		bool bRepeated = false;
		for (int32 Iteration = 0; Iteration < 100; ++Iteration)
		{
			TSoftObjectPtr<class USoundBase> Last = GameInstance->MusicSounds.Last();
			GameInstance->ReshuffleMusicPaths();
			TSoftObjectPtr<class USoundBase> First = GameInstance->MusicSounds[0];
			if (Last == First)
			{
				bRepeated = true;
				break;
			}
		}

		AssertFalse(bRepeated, TEXT("last music repeated after shuffle"));
	}

	FinishTest(EFunctionalTestResult::Default, FString());
}
//~ End AFunctionalTest Interface
