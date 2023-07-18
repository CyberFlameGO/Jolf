// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfWorldSettings.h"
#include "EngineUtils.h"
#include "LandscapeProxy.h"
#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#include "JolfHole.h"
#endif // WITH_EDITOR

AJolfWorldSettings::AJolfWorldSettings(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	KillZ = -100.f;
}

// Public Interfaces:

//~ Begin AActor Interface
#if WITH_EDITOR
void AJolfWorldSettings::CheckForErrors()
{
	Super::CheckForErrors();

	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	// We use this to keep landscape settings consistent across project.
	// The default landscape settings are not tuned for our small high-resolution components.
	for (TActorIterator<ALandscapeProxy> Iter(World); Iter; ++Iter)
	{
		ALandscapeProxy* Landscape = *Iter;
		Landscape->LOD0ScreenSize = 1.f;
		Landscape->LOD0DistributionSetting = 1.f;
		Landscape->LODDistributionSetting = 2.f;

		if (Landscape->RuntimeVirtualTextures.Num() < 1)
		{
			FMessageLog("MapCheck").Warning()
				->AddToken(FUObjectToken::Create(Landscape))
				->AddToken(FTextToken::Create(NSLOCTEXT("Jolf", "MapCheck_LandscapeRVT", "Not writing to runtime virtual textures")));
		}
	}

	int32 MinIndex = MAX_int32;
	int32 MaxIndex = MIN_int32;
	TMultiMap<int32, AJolfHole*> HoleMap;
	for (TActorIterator<AJolfHole> Iter(World); Iter; ++Iter)
	{
		HoleMap.Add(Iter->HoleIndex, *Iter);
		MinIndex = FMath::Min(MinIndex, Iter->HoleIndex);
		MaxIndex = FMath::Max(MaxIndex, Iter->HoleIndex);
	}

	if (HoleMap.Num() > 0)
	{
		TArray<AJolfHole*> TempHolesList;
		for (int32 HoleIndex = MinIndex; HoleIndex <= MaxIndex; ++HoleIndex)
		{
			TempHolesList.Reset();
			HoleMap.MultiFind(HoleIndex, TempHolesList);
			if (TempHolesList.Num() == 0)
			{
				const FText BaseText = NSLOCTEXT("Jolf", "MapCheck_HoleIndexGap", "Missing hole at index {0}");
				const FText FormattedText = FText::FormatOrdered(BaseText, FText::AsNumber(HoleIndex));
				FMessageLog("MapCheck").Error(FormattedText);
			}
			else if (TempHolesList.Num() > 1)
			{
				const FText BaseText = NSLOCTEXT("Jolf", "MapCheck_DuplicateHoles", "Multiple holes with the same hole index ({0})");
				const FText FormattedText = FText::FormatOrdered(BaseText, FText::AsNumber(HoleIndex));
				TSharedRef<FTokenizedMessage> Message = FMessageLog("MapCheck").Error(FormattedText);
				for (AJolfHole* Hole : TempHolesList)
				{
					Message->AddToken(FUObjectToken::Create(Hole));
				}
			}
		}
	}
}
#endif // WITH_EDITOR
//~ End AActor Interface