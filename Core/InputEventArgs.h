#pragma once

#include "TypeDefs.h"

namespace ORTS
{
	// Codes for input event types (sources)
	enum InputEventCode
	{
		None = 0,
		KeyPressed = 1<<0,
		KeyReleased = 1<<1,
		MouseDown = 1<<2,
		MouseUp = 1<<3,
		MouseScrollMove = 1<<4,
		MouseMove = 1<<5
	};

	// Key modifiers -> shift, control, alt
	enum KeyModifiers
	{
		None = 0,
		Control = 1,
		Shift = 2,
		Alt = 4
	};

	// Mouse buttons codes
	enum MouseButtons
	{
		None = 0,
		Left = 1,
		Right = 2,
		Middle = 4
	};

	// Base class for all input events, contains info of event type / key-button code / pressed modifiers
	class InputEventArgs
	{
	protected:
		InputEventCode _eventType;
		// Additinal info which further specifies event source ( like keycode, mouse button )
		uint16 _eventInfo;
		// Key modifiers are common for every event
		uint16 _keyModifiers;

	public:
		InputEventArgs(InputEventCode eventType, uint16 eventInfo) 
		{ 
			_eventType = eventType;
			_eventInfo = eventInfo;
		}

		inline InputEventCode GetEventType() { return _eventType; }

		// Returns code unique for event type ( that is considering not only
		// event type like KeyPressed but also specific key )
		inline uint32 GetFullEventCode() { return (((uint32)_eventType << 16) | _eventInfo); }

		// Sets / checks for pressed key modifiers
		inline bool CheckForModifier(KeyModifiers modifier) { return (bool)(_keyModifiers & modifier); }
		inline void SetModifier(KeyModifiers modifier) { _keyModifiers |= modifier; }
		inline void ResetModifier(KeyModifiers modifier) { _keyModifiers &= ~(uint16)modifier; }
	};

	// Mouse move is special event when mouse pos is needed
	// Also stores pressed buttons
	// It is possible to request event callback only when specified button is pressed :
	// call constructor with MouseButtons value (None (default) for every button)
	class MouseMoveEventArgs : public InputEventArgs
	{
	public:
		// Position on screen in range of [0:1]
		float xPos;
		float yPos;
		uint16 buttonsPressed;

	public:
		MouseMoveEventArgs(MouseButtons pressedNeeded = MouseButtons::None) : 
			InputEventArgs(InputEventCode::MouseMove, (uint16)pressedNeeded)
		{ }

		inline bool CheckIfButtonPressed(MouseButtons button) { return (bool)(buttonsPressed & (uint16)button); }
		inline void SetButtonPressed(MouseButtons button) { buttonsPressed |= (uint16)button; }
		inline void ResetButtonPressed(MouseButtons button) { buttonsPressed &= ~(uint16)button; }
	};

	// Scroll moved is special event when delta is needed
	class MouseScrollMoveEventArgs : public InputEventArgs
	{
	public:
		// Delta ( scroll change positive is 'down' direction ?? )
		float delta;

	public:
		MouseScrollMoveEventArgs() : 
			InputEventArgs(InputEventCode::MouseScrollMove, 0)
		{ }
	};
}