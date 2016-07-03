#include "InputEvents.h"
#include <InputManager.h>

namespace ORTS
{
	static InputManager* inputMgr = InputManager::Instance();

	bool KeyPressedEvent::CheckEventOccured()
	{
		if(_modifiers != 0)
		{
			// Check modifiers
			if( _modifiers & KeyModifiers::Shift != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Shift)) )
				return false;
			if( _modifiers & KeyModifiers::Ctrl != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Ctrl)) )
				return false;
			if( _modifiers & KeyModifiers::Alt != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Alt)) )
				return false;
		}
		return inputMgr->CheckKeyWasPressed((KeyCode)_keyCode);
	}

	bool KeyReleasedEvent::CheckEventOccured()
	{
		if(_modifiers != 0)
		{
			// Check modifiers
			if( _modifiers & KeyModifiers::Shift != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Shift)) )
				return false;
			if( _modifiers & KeyModifiers::Ctrl != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Ctrl)) )
				return false;
			if( _modifiers & KeyModifiers::Alt != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Alt)) )
				return false;
		}
		return inputMgr->CheckKeyWasReleased((KeyCode)_keyCode);
	}

	bool MousePressedEvent::CheckEventOccured()
	{
		if(_modifiers != 0)
		{
			// Check modifiers
			if( _modifiers & KeyModifiers::Shift != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Shift)) )
				return false;
			if( _modifiers & KeyModifiers::Ctrl != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Ctrl)) )
				return false;
			if( _modifiers & KeyModifiers::Alt != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Alt)) )
				return false;
		}
		return inputMgr->CheckMouseButtonWasPressed((MouseButton)_button);
	}

	bool MouseReleasedEvent::CheckEventOccured()
	{
		if(_modifiers != 0)
		{
			// Check modifiers
			if( _modifiers & KeyModifiers::Shift != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Shift)) )
				return false;
			if( _modifiers & KeyModifiers::Ctrl != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Ctrl)) )
				return false;
			if( _modifiers & KeyModifiers::Alt != 0 &&
				!inputMgr->CheckModifierIsDown((ModifierCode)(_modifiers & OIS::Keyboard::Alt)) )
				return false;
		}
		return inputMgr->CheckMouseButtonWasReleased((MouseButton)_button);
	}

	bool MouseClickedEvent::CheckEventOccured()
	{
		return false;
	}

	bool MouseDoubleClickedEvent::CheckEventOccured()
	{
		return false;
	}

	bool MouseMovedEvent::CheckEventOccured()
	{
		return false;
	}

	bool MouseEnterRegionEvent::CheckEventOccured()
	{
		return false;
	}

	bool MouseLeaveRegionEvent::CheckEventOccured()
	{
		return false;
	}
}
