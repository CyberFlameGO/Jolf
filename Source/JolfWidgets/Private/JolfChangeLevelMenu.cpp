// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfChangeLevelMenu.h"
#include "JolfMenuStackRoot.h"
#include "Components/Button.h"
#include "JolfChangeLevelButton.h"
#include "JolfLevelDefinition.h"
#include "Engine/World.h"
#include "JolfPlayerController.h"
#include "JolfLevelsMenu.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfChangeLevelMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(ChangeLevelButton))
		ChangeLevelButton->OnClicked.AddUObject(this, &UJolfChangeLevelMenu::OnChangeLevelClicked);

	if (ensure(TravelButton))
		TravelButton->OnClicked.AddDynamic(this, &UJolfChangeLevelMenu::OnTravelClicked);

	if (ensure(BackButton))
		BackButton->OnClicked.AddDynamic(this, &UJolfChangeLevelMenu::OnBackClicked);
}

void UJolfChangeLevelMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UWorld* World = GetWorld();
	if (ensure(ChangeLevelButton) && ensure(World))
	{
		UJolfLevelDefinition* LevelDef = UJolfLevelDefinition::FindByWorld(*World);
		ChangeLevelButton->SetLevelDef(LevelDef);
	}

	UJolfLevelsMenu* LevelsMenu = GetMenuRoot()->FindMenuByClass<UJolfLevelsMenu>();
	if (ensure(LevelsMenu))
	{
		LevelsMenu->OnSelectionChanged.AddUObject(this, &UJolfChangeLevelMenu::OnLevelSelectionChanged);
	}
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfChangeLevelMenu::OnChangeLevelClicked()
{
	GetMenuRoot()->SetActiveMenu(TEXT("LevelSelection"));
}

void UJolfChangeLevelMenu::OnTravelClicked()
{
	AJolfPlayerController* PC = GetOwningPlayer<AJolfPlayerController>();
	if (!ensure(PC))
		return;

	const UJolfLevelDefinition* LevelDef = ChangeLevelButton->GetLevelDef();
	if (LevelDef == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot travel without level definition selected"));
		return;
	}

	PC->ServerRequestTravel(LevelDef->LevelGuid);
}

void UJolfChangeLevelMenu::OnBackClicked()
{
	GetMenuRoot()->PopMenu();
}

void UJolfChangeLevelMenu::OnLevelSelectionChanged(UJolfLevelDefinition* InLevelDef)
{
	ChangeLevelButton->SetLevelDef(InLevelDef);
}