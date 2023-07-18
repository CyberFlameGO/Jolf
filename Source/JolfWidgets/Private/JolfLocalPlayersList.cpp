// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfLocalPlayersList.h"
#include "Components/SpinBox.h"
#include "JolfUserSettings.h"
#include "Components/VerticalBox.h"
#include "JolfLocalPlayerEntry.h"
#include "Engine/GameInstance.h"

// Public Functions:

void UJolfLocalPlayersList::SetNumPlayers(int32 InNumPlayers)
{
	if (NumPlayers == InNumPlayers)
		return;

	NumPlayersSpinBox->SetValue(InNumPlayers);
	NumPlayers = InNumPlayers;

	if (NumPlayers > EntryInstances.Num())
	{
		UJolfUserSettings* SystemSettings = UJolfUserSettings::GetJolfUserSettings();
		for (int32 Index = EntryInstances.Num(); Index < NumPlayers; ++Index)
		{
			UJolfLocalPlayerEntry* EntryInstance = CreateWidget<UJolfLocalPlayerEntry>(this, EntryClass);
			if (ensure(EntryInstance))
			{
				EntryInstance->SetPlayerSettings(SystemSettings->FindOrCreatePlayerSettings(Index));
				EntryInstance->OnPlayerSettingsClicked.AddUObject(this, &UJolfLocalPlayersList::OnPlayerSettingsClicked);
				EntriesBox->AddChild(EntryInstance);
				EntryInstances.Add(EntryInstance);
			}
		}
	}
	else if (NumPlayers < EntryInstances.Num())
	{
		for (int32 Index = EntryInstances.Num() - 1; Index >= NumPlayers; --Index)
		{
			UJolfLocalPlayerEntry* EntryInstance = EntryInstances[Index];
			check(EntryInstance);
			EntryInstance->RemoveFromParent();
			EntryInstances.RemoveAt(Index);
		}
	}

	check(EntryInstances.Num() == NumPlayers);
}

void UJolfLocalPlayersList::SetPlayerSettings(int32 InControllerId, UJolfPerPlayerSettings* InPlayerSettings)
{
	UGameInstance* GameInstance = GetGameInstance();
	check(GameInstance);
	if (UJolfLocalPlayer* LocalPlayer = CastChecked<UJolfLocalPlayer>(GameInstance->GetLocalPlayerByIndex(InControllerId), ECastCheckedType::NullAllowed))
	{
		// Primary player already exists on menu.
		LocalPlayer->SetPlayerSettings(InPlayerSettings);
	}
	else
	{
		// Secondary players do not exist on menu.
		UJolfUserSettings* SystemSettings = UJolfUserSettings::GetJolfUserSettings();
		SystemSettings->SetPlayerSettingsAssociation(InControllerId, InPlayerSettings);
	}

	if (EntryInstances.IsValidIndex(InControllerId))
	{
		EntryInstances[InControllerId]->SetPlayerSettings(InPlayerSettings);
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfLocalPlayersList::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(NumPlayersSpinBox))
		NumPlayersSpinBox->OnValueCommitted.AddDynamic(this, &UJolfLocalPlayersList::OnNumPlayersSpinBoxChanged);
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfLocalPlayersList::OnNumPlayersSpinBoxChanged(float InValue, ETextCommit::Type CommitMethod)
{
	int32 RoundedValue = FMath::RoundToInt(InValue);
	if (NumPlayers != RoundedValue)
	{
		SetNumPlayers(RoundedValue);
		OnNumPlayersChanged.Broadcast(NumPlayers);
	}
}

void UJolfLocalPlayersList::OnPlayerSettingsClicked(UJolfPerPlayerSettings* InPlayerSettings)
{
	check(InPlayerSettings);
	for (int32 Index = 0; Index < EntryInstances.Num(); ++Index)
	{
		if (EntryInstances[Index]->GetPlayerSettings() == InPlayerSettings)
		{
			OnLocalPlayerClicked.Broadcast(Index, InPlayerSettings);
		}
	}
}