// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CoreMinimal.h"

struct JOLFSLATE_API FJolfSlateFocus
{
	/** Should widget hovered by mouse be drawn focused? */
	static bool bDrawMouseHover;

	/** Should focused widget be drawn focused? */
	static uint32 DrawFocusHoverMask;
};