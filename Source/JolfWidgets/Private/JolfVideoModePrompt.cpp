// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfVideoModePrompt.h"
#include "JolfUserSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UJolfVideoModePrompt::ResetPrompt(FSimpleDelegate InCallback)
{
	DecisionCallback = InCallback;
	RemainingTime = 15.f;
	bDecisionPending = true;
}

//~ Begin UUserWidget Interface
void UJolfVideoModePrompt::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ensure(ConfirmButton))
		ConfirmButton->OnClicked.AddDynamic(this, &UJolfVideoModePrompt::OnConfirmClicked);

	if (ensure(RevertButton))
		RevertButton->OnClicked.AddDynamic(this, &UJolfVideoModePrompt::OnRevertClicked);
}

void UJolfVideoModePrompt::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bDecisionPending)
		return;

	TimerLabel->SetText(FText::FormatOrdered(NSLOCTEXT("Jolf", "VideoModeTimer", "Reverting in {0} seconds..."), FText::AsNumber(FMath::CeilToInt(RemainingTime))));

	RemainingTime -= InDeltaTime;
	if (RemainingTime < 0.f)
	{
		OnRevertClicked();
	}
}
//~ End UUserWidget Interface

// Private Functions:

void UJolfVideoModePrompt::OnConfirmClicked()
{
	bDecisionPending = false;
	UJolfUserSettings::GetJolfUserSettings()->ConfirmVideoMode();
	UJolfUserSettings::GetJolfUserSettings()->LastConfirmedUserInterfaceScale = UJolfUserSettings::GetJolfUserSettings()->UserInterfaceScale;
	UJolfUserSettings::GetJolfUserSettings()->SaveSettings();
	GetMenuRoot()->PopMenu();
}

void UJolfVideoModePrompt::OnRevertClicked()
{
	bDecisionPending = false;
	UJolfUserSettings::GetJolfUserSettings()->RevertVideoMode();
	UJolfUserSettings::GetJolfUserSettings()->UserInterfaceScale = UJolfUserSettings::GetJolfUserSettings()->LastConfirmedUserInterfaceScale;
	UJolfUserSettings::GetJolfUserSettings()->ApplySettings(false);
	GetMenuRoot()->PopMenu();
	DecisionCallback.ExecuteIfBound();
}