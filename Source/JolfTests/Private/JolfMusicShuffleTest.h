// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "JolfFunctionalTest.h"
#include "JolfMusicShuffleTest.generated.h"

/** Shuffle should not repeat the same song twice in a row. */
UCLASS()
class AJolfMusicShuffleTest : public AJolfFunctionalTest
{
	GENERATED_BODY()

public:

	AJolfMusicShuffleTest(const FObjectInitializer& ObjectInitializer);

protected: // Interfaces

	//~ Begin AFunctionalTest Interface
	void StartTest() override;
	//~ End AFunctionalTest Interface
};
