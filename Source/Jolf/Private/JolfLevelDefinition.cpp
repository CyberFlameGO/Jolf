// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfLevelDefinition.h"
#include "JolfAssetManager.h"

// Public Functions:

UJolfLevelDefinition* UJolfLevelDefinition::FindByWorld(const UWorld& InWorld)
{
	return UJolfAssetManager::Get().FindLevelDefinitionByWorld(InWorld);
}

UJolfLevelDefinition* UJolfLevelDefinition::FindByGuid(const FGuid& InUniqueId)
{
	return UJolfAssetManager::Get().FindLevelDefinitionByGuid(InUniqueId);
}

// Public Interfaces:

//~ Begin UObject Interface
FPrimaryAssetId UJolfLevelDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(UJolfAssetManager::LevelDefinitionType, GetFName());
}

#if WITH_EDITOR
EDataValidationResult UJolfLevelDefinition::IsDataValid(TArray<FText>& ValidationErrors)
{
	EDataValidationResult Result = Super::IsDataValid(ValidationErrors);
	if (Result == EDataValidationResult::NotValidated)
	{
		Result = EDataValidationResult::Valid;
	}

	if (Level.IsNull())
	{
		ValidationErrors.Add(NSLOCTEXT("JolfLevelDefinition", "DataValidation_LevelNull", "level has not been assigned."));
		Result = EDataValidationResult::Invalid;
	}

	if (!LevelGuid.IsValid())
	{
		ValidationErrors.Add(NSLOCTEXT("JolfLevelDefinition", "DataValidation_GuidInvalid", "definition should be resaved to generate a guid."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif // WITH_EDITOR

void UJolfLevelDefinition::Serialize(FStructuredArchiveRecord Record)
{
#if WITH_EDITOR
	if (Record.GetUnderlyingArchive().IsSaving() && !LevelGuid.IsValid())
	{
		LevelGuid = FGuid::NewGuid();
	}
#endif // WITH_EDITOR

	Super::Serialize(Record);
}

void UJolfLevelDefinition::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
#if WITH_EDITOR
	if (DuplicateMode == EDuplicateMode::Normal)
	{
		// Duplicated the asset, so a new guid will be generated during Serialize.
		LevelGuid.Invalidate();
	}
#endif // WITH_EDITOR

	Super::PostDuplicate(DuplicateMode);
}
//~ End UObject Interface
