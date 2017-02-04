#include "Input.h"
#include "SystemSettings.h"
#include "InputSettings.h"
#include <OISKeyboard.h>

namespace FlagRTS
{

#define OIS_DOWN 1
#define OIS_UP 0

	InputState::InputState()
	{
		Reset();
	}

	void InputState::UpdateKeyStates(int8* kstates)
	{
		for(int k = 0; k < 256; k++)
		{
			// Rewrite up/down state
			_keyStatesLast[k] = _keyStates[k] & (InputStates::Down|InputStates::Up);

			// Check if key is now down, was up -> so its pressed
			if( kstates[k] == OIS_DOWN  )
			{
				_keyStates[k] = InputStates::Down;
				if(_keyStatesLast[k] & InputStates::Up )
				{
					_keyStates[k] |= InputStates::Pressed;
				}
			}
			// Check if key is now up, was down -> so its released
			else if( kstates[k] == OIS_UP  )
			{
				_keyStates[k] = InputStates::Up;
				if(_keyStatesLast[k] & InputStates::Down )
				{
					_keyStates[k] |= InputStates::Released;
				}
			}
		}
	}

	void InputState::UpdateMouseStates(int8* mstates)
	{
		for(int k = 0; k < 3; k++)
		{
			_mouseStatesLast[k] = _mouseStates[k];
			_mouseStates[k] = mstates[k];
			
			// Check if key is now down, was up -> so its pressed
			if( mstates[k] & InputStates::Down )
			{
				if( _mouseStatesLast[k] & InputStates::Up )
				{
					_mouseStates[k] |= InputStates::Pressed;
					// Start click timer
					if(_click)
					{
						_dclick = true;
					}

					_click = true;
					_clickTimer = 0.0f;
				}
			}
			// Check if key is now up, was down -> so its released
			if( mstates[k] & InputStates::Up )
			{
				if( _mouseStatesLast[k] & InputStates::Down )
				{
					_mouseStates[k] |= InputStates::Released;

					if(_dclick && _doubleClickTimer <
						SystemSettings::GetInputSettings().DoubleClickInterval)
					{
						_mouseStates[k] |= InputStates::DoubleClicked;
					}
					_dclick = false;

					if(_click && _clickTimer < 
						SystemSettings::GetInputSettings().ClickInterval)
					{
						_mouseStates[k] |= InputStates::Clicked;
						_doubleClickTimer = 0.0f;
					}
					else
						_click = false;
				}
			}
		}
	}

	void InputState::UpdateMouseScreenPos(const Vector2& pos)
	{
		_mouseScreenPosLast = _mouseScreenPos;
		_mouseScreenPos = pos;
		_mouseScreenPosChange = _mouseScreenPos - _mouseScreenPosLast;
	}

	void InputState::UpdateMouseScreenPosAbs(const Vector2& pos)
	{
		_mouseScreenPosAbsLast = _mouseScreenPosAbs;
		_mouseScreenPosAbs = pos;
		_mouseScreenPosAbsChange = _mouseScreenPosAbs - _mouseScreenPosAbsLast;
	}

	void InputState::UpdateMouseRealPos(const Vector3& pos)
	{
		_mouseRealPosLast = _mouseRealPos;
		_mouseRealPos = pos;
		_mouseRealPosChange = _mouseRealPos - _mouseRealPosLast;
	}

	void InputState::UpdateMouseTerrainPos(const Vector3& pos)
	{
		_mouseTerrainPosLast = _mouseTerrainPos;
		_mouseTerrainPos = pos;
		_mouseTerrainPosChange = _mouseTerrainPos - _mouseTerrainPosLast;
	}

	void InputState::UpdateScrollChange(float scroll)
	{
		_scrollChange = scroll - _oldScroll;
		_oldScroll = scroll;
	}

	void InputState::UpdateClickTimer(double ms)
	{
		if(_click)
		{
			_clickTimer += (float)ms;
			_doubleClickTimer += (float)ms;
		}
	}

	// Reset all states to default values ( call if input is lost )
	void InputState::Reset()
	{
		for(int k = 0; k < 256; k++)
		{
			_keyStatesLast[k] = _keyStates[k] = InputStates::Up;
		}

		for(int k = 0; k < 3; k++)
		{
			_mouseStatesLast[k] = _mouseStates[k] = InputStates::Up;
		}

		_mouseScreenPos = Vector2(0.0f,0.0f);
		_mouseScreenPosLast = Vector2(0.0f,0.0f);
		_mouseScreenPosChange = Vector2(0.0f,0.0f);
		_mouseScreenPosAbs = Vector2(0.0f,0.0f);
		_mouseScreenPosAbsLast = Vector2(0.0f,0.0f);
		_mouseScreenPosAbsChange = Vector2(0.0f,0.0f);

		_mouseRealPos = Vector3(0.0f,0.0f,0.0f);
		_mouseRealPosLast = Vector3(0.0f,0.0f,0.0f);
		_mouseRealPosChange = Vector3(0.0f,0.0f,0.0f);
		_mouseTerrainPos = Vector3(0.0f,0.0f,0.0f);
		_mouseTerrainPosLast = Vector3(0.0f,0.0f,0.0f);
		_mouseTerrainPosChange = Vector3(0.0f,0.0f,0.0f);

		_doubleClickTimer = 0.0f;
		_clickTimer = 0.0f;
	}
}