#pragma once

#include "TypeDefs.h"
#include <OgreVector2.h>
#include <OgreVector3.h>

namespace FlagRTS
{
	typedef int KeyCode;
	typedef int MouseButton;
	namespace MouseButtons
	{
		enum MB : int
		{
			Left = 0,
			Right = 1,
			Middle = 2
		};
	}

	typedef int ModifierCode;
	namespace ModifierCodes
	{
		enum MC
		{
			Shift = 1 << 0,
			Ctrl = 1 << 4,
			Alt = 1 << 8
		};
	}

	namespace InputStates
	{
		enum IS : int8
		{
			Up = 1<<1,
			Down = 1<<2,
			Pressed = 1<<3,
			Released = 1<<4,
			Clicked = 1<<5,
			DoubleClicked = 1<<6,

			Unknown = 0
		};
	}

	class InputState
	{
	private:
		int8 _keyStates[256];
		int8 _keyStatesLast[256];
		int8 _mouseStates[3];
		int8 _mouseStatesLast[3];

		Vector2 _mouseScreenPos;
		Vector2 _mouseScreenPosLast;
		Vector2 _mouseScreenPosChange;
		Vector2 _mouseScreenPosAbs;
		Vector2 _mouseScreenPosAbsLast;
		Vector2 _mouseScreenPosAbsChange;
		Vector3 _mouseRealPos;
		Vector3 _mouseRealPosLast;
		Vector3 _mouseRealPosChange;
		Vector3 _mouseTerrainPos;
		Vector3 _mouseTerrainPosLast;
		Vector3 _mouseTerrainPosChange;

		float _oldScroll;
		float _scrollChange;

		float _clickTimer;
		bool _click;
		float _doubleClickTimer;
		bool _dclick;

	public:
		InputState();

		const int8* GetKeyStates() const { return _keyStates; }
		const int8* GetKeyStatesLast() const { return _keyStatesLast; }
		 int8* GetKeyStates()  { return _keyStates; }
		 int8* GetKeyStatesLast()  { return _keyStatesLast; }

		const int8* GetMouseStates() const { return _mouseStates; }
		const int8* GetMouseStatesLast() const { return _mouseStatesLast; }
		 int8* GetMouseStates()  { return _mouseStates; }
		 int8* GetMouseStatesLast()  { return _mouseStatesLast; }

		const Vector2& GetMouseScreenPos() const { return _mouseScreenPos; }
		const Vector2& GetMouseScreenPosLast() const { return _mouseScreenPosLast; }
		const Vector2& GetMouseScreenPosChange() const { return _mouseScreenPosChange; }
		 Vector2& GetMouseScreenPos()  { return _mouseScreenPos; }
		 Vector2& GetMouseScreenPosLast()  { return _mouseScreenPosLast; }
		 Vector2& GetMouseScreenPosChange()  { return _mouseScreenPosChange; }

		 const Vector2& GetMouseScreenPosAbs() const { return _mouseScreenPosAbs; }
		const Vector2& GetMouseScreenPosAbsLast() const { return _mouseScreenPosAbsLast; }
		const Vector2& GetMouseScreenPosAbsChange() const { return _mouseScreenPosAbsChange; }
		 Vector2& GetMouseScreenAbsPos()  { return _mouseScreenPosAbs; }
		 Vector2& GetMouseScreenAbsPosLast()  { return _mouseScreenPosAbsLast; }
		 Vector2& GetMouseScreenAbsPosChange()  { return _mouseScreenPosAbsChange; }

		const Vector3& GetMouseRealPos() const { return _mouseRealPos; }
		const Vector3& GetMouseRealPosLast() const { return _mouseRealPosLast; }
		const Vector3& GetMouseRealPosChange() const { return _mouseRealPosChange; }
		 Vector3& GetMouseRealPos()  { return _mouseRealPos; }
		 Vector3& GetMouseRealPosLast()  { return _mouseRealPosLast; }
		 Vector3& GetMouseRealPosChange()  { return _mouseRealPosChange; }

		const Vector3& GetMouseTerrainPos() const { return _mouseTerrainPos; }
		const Vector3& GetMouseTerrainPosLast() const { return _mouseTerrainPosLast; }
		const Vector3& GetMouseTerrainPosChange() const { return _mouseTerrainPosChange; }
		 Vector3& GetMouseTerrainPos()  { return _mouseTerrainPos; }
		 Vector3& GetMouseTerrainPosLast()  { return _mouseTerrainPosLast; }
		 Vector3& GetMouseTerrainPosChange()  { return _mouseTerrainPosChange; }

		bool CheckMouseMoved() const { return _mouseScreenPos != _mouseScreenPosLast; }

		float GetScrollChange() const { return _scrollChange; }
		float& GetScrollChange()  { return _scrollChange; }

		void UpdateKeyStates(int8*);
		void UpdateMouseStates(int8*);
		void UpdateMouseScreenPos(const Vector2& pos);
		void UpdateMouseScreenPosAbs(const Vector2& pos);
		void UpdateMouseRealPos(const Vector3& pos); // Updated not via InputManager, but CameraController
		void UpdateMouseTerrainPos(const Vector3& pos); // Updated not via InputManager, but CameraController
		void UpdateScrollChange(float scroll);
		void UpdateClickTimer(double ms);

		// Reset all states to default values ( call if input is lost )
		void Reset();

		// Checks if key is down (right now)
		int8 CheckKeyIsDown(KeyCode keyCode) const 
		{ 
			return GetKeyStates()[keyCode] & InputStates::Down; 
		}
		
		// Checks if button is down (right now)
		int8 CheckMouseButtonIsDown(MouseButton button) const
		{
			return GetMouseStates()[button] & InputStates::Down;
		}

		// Checks if key is up (right now)
		int8 CheckKeyIsUp(KeyCode keyCode) const 
		{ 
			return GetKeyStates()[keyCode] & InputStates::Up; 
		}
		
		// Checks if button is up (right now)
		int8 CheckMouseButtonIsUp(MouseButton button) const
		{
			return GetMouseStates()[button] & InputStates::Up;
		}

		// Check if key was pressed in this frame
		int8 CheckKeyWasPressed(KeyCode keyCode) const
		{
			return GetKeyStates()[keyCode] & InputStates::Pressed;
		}

		// Check if key was released in this frame
		int8 CheckKeyWasReleased(KeyCode keyCode) const
		{
			return GetKeyStates()[keyCode] & InputStates::Released;
		}

		// Check if mbutton was pressed in this frame
		int8 CheckMouseWasPressed(MouseButton button) const
		{
			return GetMouseStates()[button] & InputStates::Pressed;
		}
		
		// Check if mbutton was released in this frame
		int8 CheckMouseWasReleased(MouseButton button) const
		{
			return GetMouseStates()[button] & InputStates::Released;
		}

		// Check if mbutton was released in this frame
		int8 CheckMouseWasClicked(MouseButton button) const
		{
			return GetMouseStates()[button] & InputStates::Clicked;
		}

		// Check if mbutton was released in this frame
		int8 CheckMouseWasDoubleClicked(MouseButton button) const
		{
			return GetMouseStates()[button] & InputStates::DoubleClicked;
		}
	};
}