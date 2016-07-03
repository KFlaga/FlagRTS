#pragma once

#include "InputEventArgs.h"

namespace ORTS
{
	class IInputListener;
	// Universal input callback for keyboard/mouse or virtual software input events
	typedef void(IInputListener::*InputEventHandler)(InputEventArgs&);

	// Input listener 'interface' to be overriden by class requesting input callbacks
	// In fact there's no members as it is needed only for correct callback registering
	// Actual input requests are registered in InputManager
	class IInputListener
	{
	public:
		IInputListener() { }
	};
}