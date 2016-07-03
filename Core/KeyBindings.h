#pragma once

#include "TypeDefs.h"
#include <map>
#include "Xml.h"
#include <OISKeyboard.h>

namespace FlagRTS
{
	// Defines all possible actions with key bind to it
	// ( except in-game defined abilities/commands hotkeys )
	namespace KeyActions
	{
	enum _KeyAction : uint16
	{
		KA_CameraCenter,
		KA_CameraLeft,
		KA_CameraRight,
		KA_CameraUp,
		KA_CameraDown,
		KA_CameraZoomIn,
		KA_CameraZoomOut,
		KA_CameraPitchUp,
		KA_CameraPitchDown,
		KA_CameraYawLeft,
		KA_CameraYawRight,
	};
	}
	typedef KeyActions::_KeyAction KeyAction;

	// Contain info on current key-bindings ( except
	// unit specfic hotkeys etc, that are game content dependent )
	// Key bindings contains : ActionType ( int ), ActionName ( string ) and KeyCode ( int )
	// ActionType is used in-game, ActionName is used in Options and cfg ( becouse are human-friendly )
	class KeyBindings
	{
	public:
		KeyBindings(); // Sets default values
		void Load(const string& keyConfig); // Reads all keys from config file
		// Map that binds keycodes to actions - for use in-game
		std::map<KeyAction, int> KeyActionMap;
		// Map that binds KeyAcions ( from KeyActionMap ) with string names
		std::map<string, std::map<KeyAction, int>::iterator> KeyActionNamesMap;
		// Maps key names ( from file ) to KeyCodes
		std::map<string, int> KeyCodeNamesMap;
		// Maps key codes to ASCII
		std::map<int, unsigned char> KeyCodeToASCIIMap;

		bool CheckKeyIsBind(int keyCode);
		bool CheckActionIsBind(int KeyAction);
	};
}