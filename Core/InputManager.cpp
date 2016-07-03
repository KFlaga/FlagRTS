#include "InputManager.h"
#include <Ogre.h>
#include "LogManager.h"
#include "RenderSystemManager.h"
#include <OGRE\Ogre.h>
#include <OIS\OISMouse.h>
#include <OIS\OISKeyboard.h>
#include <OIS\OISJoyStick.h>
#include <OISInputManager.h>
#include "SystemSettings.h"
#include "InputSettings.h"
#include "Exception.h"

namespace FlagRTS
{
	InputManager* InputManager::_instance = 0;

	InputManager::InputManager() :
		_oisManager(0),
		_keyboard(0),
		_mouse(0)
	{

	}

	InputManager::~InputManager()
	{
		_oisManager->destroyInputObject(_keyboard);
		_keyboard = 0;

		_oisManager->destroyInputObject(_mouse);
		_mouse = 0;

		OIS::InputManager::destroyInputSystem(_oisManager);
		_oisManager = 0;

	}

	// Creates OIS input manager and hooks this manager to ois system for input callback receiving
	void InputManager::InitOIS(Ogre::RenderWindow* window)
	{
		ADD_LOG("Begin InputManager::InitOIS");
		window->getCustomAttribute("WINDOW",&_hWnd);

		OIS::ParamList pl;
		std::ostringstream wnd;
		wnd << _hWnd;
		pl.insert(std::make_pair( std::string("WINDOW"), wnd.str() ));
		//pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		//pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

		_oisManager = OIS::InputManager::createInputSystem(pl);
		// Get keyboard and mouse and register for events callback
		if (_oisManager->getNumberOfDevices(OIS::OISKeyboard) > 0) 
		{
			ADD_LOG("Keyboard found");
			_keyboard = static_cast<OIS::Keyboard*>( _oisManager->createInputObject( OIS::OISKeyboard, false ) );
			_keyboard->copyKeyStates((char*)_keyStates);
		}

		if (_oisManager->getNumberOfDevices(OIS::OISMouse) > 0) 
		{
			ADD_LOG("Mouse found");
			_mouse = static_cast<OIS::Mouse*>( _oisManager->createInputObject( OIS::OISMouse, false ) );

			_mouseStates[0] = 0;
			_mouseStates[1] = 0;
			_mouseStates[2] = 0;

			ResizeInputWindow(window->getWidth(), window->getHeight());
		}

		auto resizeInput = [this](std::pair<int,int> size) { this->ResizeInputWindow(size.first,size.second); };
		typedef DelegateEventHandler<decltype(resizeInput), std::pair<int,int>> ResizeInputDelegate;
		RenderSystemManager::Instance()->WindowResized() += xNew1(ResizeInputDelegate,resizeInput);
	}

	void InputManager::Update(float ms)
	{
		//if( (CheckMouseWasPressed(OIS::MB_Left) ||
		//	CheckMouseWasPressed(OIS::MB_Right) ||
		//	CheckMouseWasPressed(OIS::MB_Middle)) &&
		//	!IsMouseWithinClient()) 
		//{
		//	// If mouse is outside client area, loose focus
		//	POINT mpos;
		//	::GetCursorPos(&mpos);
		//	HWND newWindow = ::WindowFromPoint(mpos);
		//	::ReleaseCapture();

		//	_inputState.Reset();

		//	if(newWindow != NULL)
		//		::SetWindowPos(newWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		//}
		if(::GetForegroundWindow() == (HWND)_hWnd)
		{
			// Capture and update mouse/keyboard on every frame
			if( _keyboard ) 
			{
				_keyboard->capture();
				// update button states
				_keyboard->copyKeyStates((char*)_keyStates);
				_inputState.UpdateKeyStates(_keyStates);
			}

			if( _mouse )	
			{
				_mouse->capture();

				// Get top-left of game window relative to screen
				POINT winTopLeft = { 0, 0 };
				ClientToScreen((HWND)_hWnd, &winTopLeft);
				// Get size of game window
				RECT winSize = { 0, 0, 0, 0 };
				GetClientRect((HWND)_hWnd, &winSize);

				// Get position of cursor relative to screen
				POINT cursorScreenPos = { 0, 0 };
				GetCursorPos(&cursorScreenPos);
				// Get position of cursor relative to client
				Vector2 mousePosAbs(cursorScreenPos.x - winTopLeft.x, cursorScreenPos.y - winTopLeft.y);

				Vector2 mousePos((float)mousePosAbs.x/((float)winSize.right-(float)winSize.left), 
					(float)mousePosAbs.y/((float)winSize.bottom-(float)winSize.top));

				auto mstate = _mouse->getMouseState();
				float scroll = (float)mstate.Z.abs;

				_mouseStates[0] = mstate.buttonDown(OIS::MB_Left) ? InputStates::Down : InputStates::Up;
				_mouseStates[1] = mstate.buttonDown(OIS::MB_Right) ? InputStates::Down : InputStates::Up;
				_mouseStates[2] = mstate.buttonDown(OIS::MB_Middle) ? InputStates::Down : InputStates::Up;

				_inputState.UpdateMouseStates(_mouseStates);
				_inputState.UpdateMouseScreenPos(mousePos);
				_inputState.UpdateMouseScreenPosAbs(mousePosAbs);
				_inputState.UpdateScrollChange(scroll);
				_inputState.UpdateClickTimer(ms);

				//After computations change WinAPI cursor position 
				//( as in game it will be shown by WinAPI )
				//if( IsMouseWithinClient() )
				//	SetWinCursorPosition();
			}
		}
	}

	void InputManager::ResizeInputWindow(int w, int h)
	{
		if(_mouse != 0)
		{
			const OIS::MouseState &mouseState = _mouse->getMouseState();
			mouseState.width  = w;
			mouseState.height = h;
		}
	}

	bool InputManager::CheckModifierIsDown(ModifierCode mod) const
	{
		return _keyboard->isModifierDown((OIS::Keyboard::Modifier)mod);
	}

	bool InputManager::IsMouseWithinClient()
	{
		// Get top-left of game window relative to screen
		POINT point = { 0, 0 };
		ClientToScreen((HWND)_hWnd, &point);
		// Get size of game window
		RECT client_rect = { 0, 0, 0, 0 };
		GetWindowRect((HWND)_hWnd, &client_rect);

		// Get position of cursor relative to screen
		POINT cursor_point = { 0, 0 };
		GetCursorPos(&cursor_point);
		// Check if cursor is within window
		bool hor = cursor_point.x >= client_rect.left && cursor_point.x <= client_rect.right;
		bool ver = cursor_point.y >= client_rect.top && cursor_point.y <= client_rect.bottom;
		return hor && ver;
	}


	void InputManager::SetWinCursorPosition()
	{
		// Input state multiplies our change by sensitivity
		Vector2 actualMPos = _inputState.GetMouseScreenAbsPos();
		POINT point = { 0, 0 };
		ClientToScreen((HWND)_hWnd, &point);
		::SetCursorPos((int)actualMPos.x + point.x, (int)actualMPos.y + point.y);
	}
}