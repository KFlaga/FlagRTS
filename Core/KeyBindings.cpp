#include "KeyBindings.h"
#include "ConfigurationManager.h"

namespace FlagRTS
{
	void KeyBindings::Load(const string& confName)
	{
		auto keyConf = ConfigurationManager::Instance()->GetConfiguration(confName);
		SectionIterator keyIt;
		SectionIterator keyEnd;
		keyConf->Begin("KeyBindings", keyIt);
		keyConf->End("KeyBindings", keyEnd);
		for(;keyIt != keyEnd; keyIt++)
		{
			// Name = KeyActionName, Value = KeyCodeName
			int keycode = KeyCodeNamesMap.find((*keyIt).Value->Value)->second;
			// Set correct value in key maps ( get it to keyaction-keycode with name, then set keycode)
			KeyActionNamesMap.find((*keyIt).Value->Name)->second->second = keycode;
		}
	}

	KeyBindings::KeyBindings()
	{
		// Set default values in all maps ( names are constant, only KeyCodes bindings may change )
		KeyCodeNamesMap["0"] = OIS::KC_0;
		KeyCodeNamesMap["1"] = OIS::KC_1;
		KeyCodeNamesMap["2"] = OIS::KC_2;
		KeyCodeNamesMap["3"] = OIS::KC_3;
		KeyCodeNamesMap["4"] = OIS::KC_4;
		KeyCodeNamesMap["5"] = OIS::KC_5;
		KeyCodeNamesMap["6"] = OIS::KC_6;
		KeyCodeNamesMap["7"] = OIS::KC_7;
		KeyCodeNamesMap["8"] = OIS::KC_8;
		KeyCodeNamesMap["9"] = OIS::KC_9;
		KeyCodeNamesMap["-"] = OIS::KC_MINUS;
		KeyCodeNamesMap["="] = OIS::KC_EQUALS;
		KeyCodeNamesMap["q"] = OIS::KC_Q;
		KeyCodeNamesMap["w"] = OIS::KC_W;
		KeyCodeNamesMap["e"] = OIS::KC_E;
		KeyCodeNamesMap["r"] = OIS::KC_R;
		KeyCodeNamesMap["t"] = OIS::KC_T;
		KeyCodeNamesMap["y"] = OIS::KC_Y;
		KeyCodeNamesMap["u"] = OIS::KC_U;
		KeyCodeNamesMap["i"] = OIS::KC_I;
		KeyCodeNamesMap["o"] = OIS::KC_O;
		KeyCodeNamesMap["p"] = OIS::KC_P;
		KeyCodeNamesMap["LBracket"] = OIS::KC_LBRACKET;
		KeyCodeNamesMap["RBracket"] = OIS::KC_RBRACKET;
		KeyCodeNamesMap["a"] = OIS::KC_A;
		KeyCodeNamesMap["s"] = OIS::KC_S;
		KeyCodeNamesMap["d"] = OIS::KC_D;
		KeyCodeNamesMap["f"] = OIS::KC_F;
		KeyCodeNamesMap["g"] = OIS::KC_G;
		KeyCodeNamesMap["h"] = OIS::KC_H;
		KeyCodeNamesMap["j"] = OIS::KC_J;
		KeyCodeNamesMap["k"] = OIS::KC_K;
		KeyCodeNamesMap["l"] = OIS::KC_L;
		KeyCodeNamesMap["Semicolon"] = OIS::KC_SEMICOLON;
		KeyCodeNamesMap["Apostrophe"] = OIS::KC_APOSTROPHE;
		KeyCodeNamesMap["z"] = OIS::KC_Z;
		KeyCodeNamesMap["x"] = OIS::KC_X;
		KeyCodeNamesMap["c"] = OIS::KC_C;
		KeyCodeNamesMap["v"] = OIS::KC_V;
		KeyCodeNamesMap["b"] = OIS::KC_B;
		KeyCodeNamesMap["n"] = OIS::KC_N;
		KeyCodeNamesMap["m"] = OIS::KC_M;
		KeyCodeNamesMap["Comma"] = OIS::KC_COMMA;
		KeyCodeNamesMap["Period"] = OIS::KC_PERIOD;
		KeyCodeNamesMap["Slash"] = OIS::KC_SLASH;
		KeyCodeNamesMap["Grave"] = OIS::KC_GRAVE;
		KeyCodeNamesMap["Tab"] = OIS::KC_TAB;
		KeyCodeNamesMap["Caps"] = OIS::KC_CAPITAL;
		KeyCodeNamesMap["LShift"] = OIS::KC_LSHIFT;
		KeyCodeNamesMap["RShift"] = OIS::KC_RSHIFT;
		KeyCodeNamesMap["LCtrl"] = OIS::KC_LCONTROL;
		KeyCodeNamesMap["RCtrl"] = OIS::KC_RCONTROL;
		KeyCodeNamesMap["LAlt"] = OIS::KC_LMENU;
		KeyCodeNamesMap["RAlt"] = OIS::KC_RMENU;
		KeyCodeNamesMap["Space"] = OIS::KC_SPACE;
		KeyCodeNamesMap["Backspace"] = OIS::KC_BACK;
		KeyCodeNamesMap["BackSlash"] = OIS::KC_BACKSLASH;
		KeyCodeNamesMap["Enter"] = OIS::KC_RETURN;
		KeyCodeNamesMap["ALeft"] = OIS::KC_LEFT;
		KeyCodeNamesMap["AUp"] = OIS::KC_UP;
		KeyCodeNamesMap["ARight"] = OIS::KC_RIGHT;
		KeyCodeNamesMap["ADown"] = OIS::KC_DOWN;
		KeyCodeNamesMap["F1"] = OIS::KC_F1;
		KeyCodeNamesMap["F2"] = OIS::KC_F2;
		KeyCodeNamesMap["F3"] = OIS::KC_F3;
		KeyCodeNamesMap["F4"] = OIS::KC_F4;
		KeyCodeNamesMap["F5"] = OIS::KC_F5;
		KeyCodeNamesMap["F6"] = OIS::KC_F6;
		KeyCodeNamesMap["F7"] = OIS::KC_F7;
		KeyCodeNamesMap["F8"] = OIS::KC_F8;
		KeyCodeNamesMap["F9"] = OIS::KC_F9;
		KeyCodeNamesMap["F10"] = OIS::KC_F10;
		KeyCodeNamesMap["F11"] = OIS::KC_F11;
		KeyCodeNamesMap["F12"] = OIS::KC_F12;
		KeyCodeNamesMap["Num0"] = OIS::KC_NUMPAD0;
		KeyCodeNamesMap["Num1"] = OIS::KC_NUMPAD1;
		KeyCodeNamesMap["Num2"] = OIS::KC_NUMPAD2;
		KeyCodeNamesMap["Num3"] = OIS::KC_NUMPAD3;
		KeyCodeNamesMap["Num4"] = OIS::KC_NUMPAD4;
		KeyCodeNamesMap["Num5"] = OIS::KC_NUMPAD5;
		KeyCodeNamesMap["Num6"] = OIS::KC_NUMPAD6;
		KeyCodeNamesMap["Num7"] = OIS::KC_NUMPAD7;
		KeyCodeNamesMap["Num8"] = OIS::KC_NUMPAD8;
		KeyCodeNamesMap["Num9"] = OIS::KC_NUMPAD9;
		KeyCodeNamesMap["NumLock"] = OIS::KC_NUMLOCK;
		KeyCodeNamesMap["ScrollLock"] = OIS::KC_SCROLL;
		KeyCodeNamesMap["NumMinus"] = OIS::KC_SUBTRACT;
		KeyCodeNamesMap["NumPlus"] = OIS::KC_ADD;
		KeyCodeNamesMap["NumDot"] = OIS::KC_DECIMAL;
		KeyCodeNamesMap["NumStar"] = OIS::KC_MULTIPLY;
		KeyCodeNamesMap["NumSlash"] = OIS::KC_DIVIDE;
		KeyCodeNamesMap["Esc"] = OIS::KC_ESCAPE;

		// Set KeyActions
		KeyActionMap[KeyActions::KA_CameraLeft] = OIS::KC_LEFT;
		KeyActionNamesMap["CameraLeft"] = KeyActionMap.find(KeyActions::KA_CameraLeft);

		KeyActionMap[KeyActions::KA_CameraRight] = OIS::KC_RIGHT;
		KeyActionNamesMap["CameraRight"] = KeyActionMap.find(KeyActions::KA_CameraRight);

		KeyActionMap[KeyActions::KA_CameraUp] = OIS::KC_UP;
		KeyActionNamesMap["CameraUp"] = KeyActionMap.find(KeyActions::KA_CameraUp);

		KeyActionMap[KeyActions::KA_CameraDown] = OIS::KC_DOWN;
		KeyActionNamesMap["CameraDown"] = KeyActionMap.find(KeyActions::KA_CameraDown);

		KeyActionMap[KeyActions::KA_CameraPitchUp] = OIS::KC_NUMPAD8;
		KeyActionNamesMap["CameraPitchUp"] = KeyActionMap.find(KeyActions::KA_CameraPitchUp);

		KeyActionMap[KeyActions::KA_CameraPitchDown] = OIS::KC_NUMPAD2;
		KeyActionNamesMap["CameraPitchDown"] = KeyActionMap.find(KeyActions::KA_CameraPitchDown);
		
		KeyActionMap[KeyActions::KA_CameraYawLeft] = OIS::KC_NUMPAD4;
		KeyActionNamesMap["CameraYawLeft"] = KeyActionMap.find(KeyActions::KA_CameraYawLeft);

		KeyActionMap[KeyActions::KA_CameraYawRight] = OIS::KC_NUMPAD6;
		KeyActionNamesMap["CameraYawRight"] = KeyActionMap.find(KeyActions::KA_CameraYawRight);
		
		KeyActionMap[KeyActions::KA_CameraZoomIn] = OIS::KC_NUMPAD7;
		KeyActionNamesMap["CameraZoomIn"] = KeyActionMap.find(KeyActions::KA_CameraZoomIn);
		
		KeyActionMap[KeyActions::KA_CameraZoomOut] = OIS::KC_NUMPAD9;
		KeyActionNamesMap["CameraZoomOut"] = KeyActionMap.find(KeyActions::KA_CameraZoomOut);
		
		KeyActionMap[KeyActions::KA_CameraCenter] = OIS::KC_NUMPAD5;
		KeyActionNamesMap["CameraCenter"] = KeyActionMap.find(KeyActions::KA_CameraCenter);

		KeyCodeToASCIIMap[OIS::KC_0] = '0';
		KeyCodeToASCIIMap[OIS::KC_1] = '1';
		KeyCodeToASCIIMap[OIS::KC_2] = '2';
		KeyCodeToASCIIMap[OIS::KC_3] = '3';
		KeyCodeToASCIIMap[OIS::KC_4] = '4';
		KeyCodeToASCIIMap[OIS::KC_5] = '5';
		KeyCodeToASCIIMap[OIS::KC_6] = '6';
		KeyCodeToASCIIMap[OIS::KC_7] = '7';
		KeyCodeToASCIIMap[OIS::KC_8] = '8';
		KeyCodeToASCIIMap[OIS::KC_9] = '9';
		KeyCodeToASCIIMap[OIS::KC_MINUS] = '-';
		KeyCodeToASCIIMap[OIS::KC_EQUALS] = '=';
		KeyCodeToASCIIMap[OIS::KC_Q] = 'q';
		KeyCodeToASCIIMap[OIS::KC_W] = 'w';
		KeyCodeToASCIIMap[OIS::KC_E] = 'e';
		KeyCodeToASCIIMap[OIS::KC_R] = 'r';
		KeyCodeToASCIIMap[OIS::KC_T] = 't';
		KeyCodeToASCIIMap[OIS::KC_Y] = 'y';
		KeyCodeToASCIIMap[OIS::KC_U] = 'u';
		KeyCodeToASCIIMap[OIS::KC_I] = 'i';
		KeyCodeToASCIIMap[OIS::KC_O] = 'o';
		KeyCodeToASCIIMap[OIS::KC_P] = 'p';
		KeyCodeToASCIIMap[OIS::KC_LBRACKET] = '(';
		KeyCodeToASCIIMap[OIS::KC_RBRACKET] = ')';
		KeyCodeToASCIIMap[OIS::KC_A] = 'a';
		KeyCodeToASCIIMap[OIS::KC_S] = 's';
		KeyCodeToASCIIMap[OIS::KC_D] = 'd';
		KeyCodeToASCIIMap[OIS::KC_F] = 'f';
		KeyCodeToASCIIMap[OIS::KC_G] = 'g';
		KeyCodeToASCIIMap[OIS::KC_H] = 'h';
		KeyCodeToASCIIMap[OIS::KC_J] = 'j';
		KeyCodeToASCIIMap[OIS::KC_K] = 'k';
		KeyCodeToASCIIMap[OIS::KC_L] = 'l';
		KeyCodeToASCIIMap[OIS::KC_SEMICOLON] = ';';
		KeyCodeToASCIIMap[OIS::KC_APOSTROPHE] = '\'';
		KeyCodeToASCIIMap[OIS::KC_Z] = 'z';
		KeyCodeToASCIIMap[OIS::KC_X] = 'x';
		KeyCodeToASCIIMap[OIS::KC_C] = 'c';
		KeyCodeToASCIIMap[OIS::KC_V] = 'v';
		KeyCodeToASCIIMap[OIS::KC_B] = 'b';
		KeyCodeToASCIIMap[OIS::KC_N] = 'n';
		KeyCodeToASCIIMap[OIS::KC_M] = 'm';
		KeyCodeToASCIIMap[OIS::KC_COMMA] = ',';
		KeyCodeToASCIIMap[OIS::KC_PERIOD] = '.';
		KeyCodeToASCIIMap[OIS::KC_SLASH] = '/';
		KeyCodeToASCIIMap[OIS::KC_GRAVE] = '~';
		KeyCodeToASCIIMap[OIS::KC_TAB] = '\t';
		KeyCodeToASCIIMap[OIS::KC_CAPITAL] = 0;
		KeyCodeToASCIIMap[OIS::KC_LSHIFT] = 0;
		KeyCodeToASCIIMap[OIS::KC_RSHIFT] = 0;
		KeyCodeToASCIIMap[OIS::KC_LCONTROL] = 0;
		KeyCodeToASCIIMap[OIS::KC_RCONTROL] = 0;
		KeyCodeToASCIIMap[OIS::KC_LMENU] = 0;
		KeyCodeToASCIIMap[OIS::KC_RMENU] = 0;
		KeyCodeToASCIIMap[OIS::KC_SPACE] = ' ';
		KeyCodeToASCIIMap[OIS::KC_BACK] = 0;
		KeyCodeToASCIIMap[OIS::KC_BACKSLASH] = '\\';
		KeyCodeToASCIIMap[OIS::KC_RETURN] = '\n';
		KeyCodeToASCIIMap[OIS::KC_LEFT] = 0;
		KeyCodeToASCIIMap[OIS::KC_UP] = 0;
		KeyCodeToASCIIMap[OIS::KC_RIGHT] = 'b';
		KeyCodeToASCIIMap[OIS::KC_DOWN] = 0;
		KeyCodeToASCIIMap[OIS::KC_F1] = 0;
		KeyCodeToASCIIMap[OIS::KC_F2] = 0;
		KeyCodeToASCIIMap[OIS::KC_F3] = 0;
		KeyCodeToASCIIMap[OIS::KC_F4] = 0;
		KeyCodeToASCIIMap[OIS::KC_F5] = 0;
		KeyCodeToASCIIMap[OIS::KC_F6] = 0;
		KeyCodeToASCIIMap[OIS::KC_F7] = 0;
		KeyCodeToASCIIMap[OIS::KC_F8] = 0;
		KeyCodeToASCIIMap[OIS::KC_F9] = 0;
		KeyCodeToASCIIMap[OIS::KC_F10] = 0;
		KeyCodeToASCIIMap[OIS::KC_F11] = 0;
		KeyCodeToASCIIMap[OIS::KC_F12] = 0;
		KeyCodeToASCIIMap[OIS::KC_NUMPAD0] = '0';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD1] = '1';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD2] = '2';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD3] = '3';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD4] = '4';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD5] = '5';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD6] = '6';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD7] = '7';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD8] = '8';
		KeyCodeToASCIIMap[OIS::KC_NUMPAD9] = '9';
		KeyCodeToASCIIMap[OIS::KC_NUMLOCK] = 0;
		KeyCodeToASCIIMap[OIS::KC_SCROLL] = 0;
		KeyCodeToASCIIMap[OIS::KC_SUBTRACT] = '-';
		KeyCodeToASCIIMap[OIS::KC_ADD] = '+';
		KeyCodeToASCIIMap[OIS::KC_DECIMAL] = '.';
		KeyCodeToASCIIMap[OIS::KC_MULTIPLY] = '*';
		KeyCodeToASCIIMap[OIS::KC_DIVIDE] = '/';
		KeyCodeToASCIIMap[OIS::KC_ESCAPE] = 0;
	}
}