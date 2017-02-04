#pragma once

#include "TypeDefs.h"
#include "GenericManager.h"
#include <OgreVector2.h>
#include <OgreVector3.h>

#include "Pointers.h"
#include "Input.h"

namespace OIS
{
	class Mouse;
	class Keyboard;
	class InputManager;
}

namespace Ogre
{
	class RenderWindow;
}

namespace FlagRTS
{
	// Class responsible for retrieving user input and apreading it further to listeners
	// On every system loop it checks for input events and buffers events
	// Systems can check if specified key/mbutton is pressed, or if mouse was moved
	class InputManager : public GenericManager<InputManager>
	{
	private: // internal variables
		OIS::InputManager* _oisManager;
		OIS::Keyboard* _keyboard;
		OIS::Mouse* _mouse;
		size_t _hWnd;

		InputState _inputState;

		int8 _keyStates[256];
		int8 _mouseStates[3];
		double _clickInterval; 
		double _doubleClickInterval; 

	protected:
		InputManager();
		friend GenericManager<InputManager>;

	public:
		~InputManager();

		// Initialzied OIS - must be called prior to use
		void InitOIS(Ogre::RenderWindow* window);
		// Begins / ends reciving OIS events and storing input
		void BeginInputCapture();
		void EndInputCapture();
		// Updates input - to be called every frame
		void Update(float ms);
		// Must be called when game window is resized
		void ResizeInputWindow(int width, int height);
		
		const InputState* GetInputState() const { return &_inputState; }
		InputState* GetInputState() { return &_inputState; }

		// Checks if key is down (right now)
		int8 CheckKeyIsDown(KeyCode keyCode) const 
		{ 
			return _inputState.CheckKeyIsDown(keyCode); 
		}
		
		// Checks if button is down (right now)
		int8 CheckMouseButtonIsDown(MouseButton button) const
		{
			return _inputState.CheckMouseButtonIsDown(button);
		}

		// Checks if key is up (right now)
		int8 CheckKeyIsUp(KeyCode keyCode) const 
		{ 
			return _inputState.CheckKeyIsUp(keyCode); 
		}
		
		// Checks if button is up (right now)
		int8 CheckMouseButtonIsUp(MouseButton button) const
		{
			return _inputState.CheckMouseButtonIsUp(button);
		}

		// Check if key was pressed in this frame
		int8 CheckKeyWasPressed(KeyCode keyCode) const
		{
			return _inputState.CheckKeyWasPressed(keyCode);
		}

		// Check if key was released in this frame
		int8 CheckKeyWasReleased(KeyCode keyCode) const
		{
			return _inputState.CheckKeyWasReleased(keyCode);
		}

		// Check if mbutton was pressed in this frame
		int8 CheckMouseWasPressed(MouseButton button) const
		{
			return _inputState.CheckMouseWasPressed(button);
		}
		
		// Check if mbutton was released in this frame
		int8 CheckMouseWasReleased(MouseButton button) const
		{
			return _inputState.CheckMouseWasReleased(button);
		}

		// Check if mbutton was released in this frame
		int8 CheckMouseWasClicked(MouseButton button) const
		{
			return _inputState.CheckMouseWasClicked(button);
		}

		// Check if mbutton was released in this frame
		int8 CheckMouseWasDoubleClicked(MouseButton button) const
		{
			return _inputState.CheckMouseWasDoubleClicked(button);
		}

		// Returns true if mouse changed position since last frame
		bool CheckMouseMovedSinceLastFrame() const
		{
			return _inputState.CheckMouseMoved();
		}

		const Vector2& GetMouseScreenPos() const
		{
			return _inputState.GetMouseScreenPos();
		}

		const Vector2& GetMouseScreenPosChange() const
		{
			return _inputState.GetMouseScreenPosChange();
		}

		const Vector2& GetMouseScreenPosAbs() const
		{
			return _inputState.GetMouseScreenPosAbs();
		}

		const Vector2& GetMouseScreenPosChangeAbs() const
		{
			return _inputState.GetMouseScreenPosAbsChange();
		}

		const Vector3& GetMouseRealPos() const
		{
			return _inputState.GetMouseRealPos();
		}

		const Vector3& GetMouseRealPosChange() const
		{
			return _inputState.GetMouseRealPosChange();
		}

		const Vector3& GetMouseTerrainPos() const
		{
			return _inputState.GetMouseTerrainPos();
		}

		const Vector3& GetMouseTerrainPosChange() const
		{
			return _inputState.GetMouseTerrainPosChange();
		}

		bool CheckModifierIsDown(ModifierCode mod) const;

		// Check if mouse hovers over client window
		// Uses WinAPI cursor, which may not be same as ingame mouse
		bool IsMouseWithinClient();

		void SetWinCursorPosition();
	};
}
