// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Engine/DataAsset.h"
#include "JolfLevelDefinition.generated.h"

UCLASS()
class JOLF_API UJolfLevelDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public: // Functions

	static UJolfLevelDefinition* FindByWorld(const UWorld& InWorld);
	static UJolfLevelDefinition* FindByGuid(const FGuid& InUniqueId);

public: // Interfaces

	//~ Begin UObject Interface
	FPrimaryAssetId GetPrimaryAssetId() const override;
#if WITH_EDITOR
	EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
#endif // WITH_EDITOR
	void Serialize(FStructuredArchiveRecord Record) override;
	void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;
	//~ End UObject Interface

public: // Properties

	UPROPERTY(EditAnywhere)
	FText TitleText;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UWorld> Level;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<class UTexture2D> Thumbnail;

	/**
	 * Unique map identifier. Allows levels to be moved and renamed without breaking saved games.
	 * Note: UPackage does have a Guid, but it changes on save, and PersistentGuid is editor-only.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AdvancedDisplay))
	FGuid LevelGuid;
};