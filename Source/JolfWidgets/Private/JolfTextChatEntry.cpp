// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "JolfTextChatEntry.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UJolfTextChatEntry::NativeConstruct()
{
	Super::NativeConstruct();
	PlayAnimation(FadeOut);
}

void UJolfTextChatEntry::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	RemoveFromParent();
}
//~ End UUserWidget Interface