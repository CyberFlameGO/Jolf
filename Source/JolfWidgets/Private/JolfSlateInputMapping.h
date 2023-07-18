// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Framework/Application/SlateApplication.h"

class FJolfSlateInputMapping : public ISlateInputManager
{
public: // Interfaces

	//~ Begin ISlateInputManager Interface
	int32 GetUserIndexForMouse() const override;
	int32 GetUserIndexForKeyboard() const override;
	int32 GetUserIndexForController(int32 ControllerId) const override;
	TOptional<int32> GetUserIndexForController(int32 ControllerId, FKey InKey) const override;
	//~ End ISlateInputManager Interface
};