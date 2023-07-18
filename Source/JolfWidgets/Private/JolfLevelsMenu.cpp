// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfLevelsMenu.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "JolfMenuStackRoot.h"
#include "JolfAssetManager.h"
#include "JolfLevelDefinition.h"
#include "JolfLevelEntry.h"
#include "JolfUserSettings.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfLevelsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfLevelsMenu::OnBackClicked);

	if (ensure(ScrollBox))
	{
		TArray<FAssetData> AssetDataList;
		UJolfAssetManager& AssetManager = UJolfAssetManager::Get();
		AssetManager.GetPrimaryAssetDataList(UJolfAssetManager::LevelDefinitionType, AssetDataList);
		for (const FAssetData& AssetData : AssetDataList)
		{
			if (UJolfLevelDefinition* LevelDef = Cast<UJolfLevelDefinition>(AssetData.GetAsset()))
			{
				UJolfLevelEntry* Entry = CreateWidget<UJolfLevelEntry>(this, EntryClass);
				if (Entry)
				{
					Entry->OnClicked.AddUObject(this, &UJolfLevelsMenu::OnLevelClicked);
					Entry->SetLevelDef(LevelDef);
					ScrollBox->AddChild(Entry);
					EntryWidgets.Add(Entry);
				}
			}
		}
	}
}
//~ End UUserWidget Interface

//~ Begin UJolfMenuStackContent Interface
UWidget* UJolfLevelsMenu::GetDefaultWidgetToFocus()
{
	const UJolfLevelDefinition* LevelDef = UJolfUserSettings::GetJolfUserSettings()->GetPreferredLevelDef();
	UJolfLevelEntry** Entry = EntryWidgets.FindByPredicate([LevelDef](const UJolfLevelEntry* TestEntry)
	{
		return TestEntry && TestEntry->GetLevelDef() == LevelDef;
	});

	if (Entry)
	{
		check(*Entry);
		return *Entry;
	}
	else
	{
		return BackButton;
	}
}
//~ End UJolfMenuStackContent Interface

// Private Functions:

void UJolfLevelsMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfLevelsMenu::OnLevelClicked(UJolfLevelDefinition* InLevelDef)
{
	check(InLevelDef);
	GetMenuRoot()->PopMenu();
	UJolfUserSettings::GetJolfUserSettings()->SetPreferredLevelDef(InLevelDef);
	OnSelectionChanged.Broadcast(InLevelDef);
}