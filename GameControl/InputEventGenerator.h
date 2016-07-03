#pragma once

#include <IEventGenerator.h>

namespace ORTS
{
	// Class that generates events that are fired from
	// user input ( mouse, key ). Uses InputManager to
	// check if events occured. Supports KeyPressed, 
	// KeyReleased, KeyDown, KeyUp, MouseMoved, MousePressed, 
	// MouseReleased, MouseClicked, MouseDoubleClikced, MouseScrolled,
	// 
	class InputEventGenerator : public IEventGenerator
	{
	protected:
		bool CheckIfEventOccured(TypeId eventType);
		void ProcessEventInteral(const RefPtr<ITrigger>& trigger);
	};
}