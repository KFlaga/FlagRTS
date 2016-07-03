#pragma once

#include "GenericParamterisedEvent.h"

namespace FlagRTS
{
	// Occurs when any key is pressed
	// Key code passed in EventArg_0
	class KeyPressedEvent : public ParametrizedEvent_1<int>
	{
	public:
		DEFINE_SIMPLE_EVENTFACTORY(KeyPressedEvent);
	};
	
	// Occurs when any key is released
	// Key code passed in EventArg_0
	class KeyReleasedEvent : public ParametrizedEvent_1<int>
	{
	public:
		DEFINE_SIMPLE_EVENTFACTORY(KeyReleasedEvent);
	};
	
	// Occurs when any mouse button is pressed
	// Button code passed in EventArg_0
	// Position is not passed, as it can be retrieved with a Vector3Retriever
	// ( <Vector from="CursorTerrain"/> or <Vector from="CursorScreen"/> )
	class MousePressedEvent : public ParametrizedEvent_1<int>
	{
	public:
		DEFINE_SIMPLE_EVENTFACTORY(MousePressedEvent);
	};
	
	// Occurs when any mouse button is released
	// Button code passed in EventArg_0
	class MouseReleasedEvent : public ParametrizedEvent_1<int>
	{
	public:
		DEFINE_SIMPLE_EVENTFACTORY(MousePressedEvent);
	};
	
	// Occurs when any mouse button is clicked (pressed/released fast)
	// Button code passed in EventArg_0
	class MouseClickedEvent : public ParametrizedEvent_1<int>
	{
	public:
		DEFINE_SIMPLE_EVENTFACTORY(MousePressedEvent);
	};
	
	// Occurs when any mouse button is double-clicked (pressed/released fast)
	// Button code passed in EventArg_0
	class MouseDoubleClickedEvent : public ParametrizedEvent_1<int>
	{
	public:
		DEFINE_SIMPLE_EVENTFACTORY(MousePressedEvent);
	};

	// TODO mouse enter/leave area (either on screen or on terrain)
	// Event have paramter IRegion and is checked every frame
}