#include "GUIManager.h"
#include <Ogre.h>
#include "ConfigurationManager.h"
#include "LogManager.h"
#include "ErrorManager.h"
#include "InputManager.h"
#include "SystemSettings.h"
#include "KeyBindings.h"

namespace FlagRTS
{
	GUIManager* GUIManager::_instance = 0;

	GUIManager::GUIManager() : 
		_mygui(new MyGUI::Gui()), 
		_ogre(new MyGUI::OgrePlatform()),
		_currentGui(RefPtr<GUI>::InvalidRefPointer())
	{

	}

	ErrorCode GUIManager::InitOgre(Ogre::SceneManager* sceneMgr, Ogre::RenderWindow* window)
	{
		ADD_LOG("Begin GUIManager::InitOGRE");
		(*_ogre).initialise(window, sceneMgr, "MyGUI");
		(*_mygui).initialise((*ConfigurationManager::Instance()->GetConfiguration("Resources")).
			FindOptionValue("MyGUI_CorePath"));
		ADD_LOG("End GUIManager::InitOGRE");
		return ErrorCodes::Success;
	}
		
	ErrorCode GUIManager::RegisterGUI(const std::string& guiName, RefPtr<GUI> gui)
	{
		if(_guis.count(guiName) > 0)
			return RETURN_ERROR(ErrorCodes::GuiAlreadyRegistered);

		
		_guis.insert(std::make_pair(guiName, gui));
		return ErrorCodes::Success;
	}

	ErrorCode GUIManager::UnregisterGUI(const std::string& guiName)
	{
		if(_guis.count(guiName) <= 0)
			return RETURN_ERROR(ErrorCodes::GuiNotFound);

		_guis.erase(guiName);
		return ErrorCodes::Success;
	}

	RefPtr<GUI> GUIManager::FindGUI(const std::string& guiName)
	{
		if(_guis.count(guiName) <= 0)
			return RefPtr<GUI>();

		return _guis.find(guiName)->second;
	}

	ErrorCode GUIManager::SetCurrentGUI(const std::string& guiName)
	{
		ADD_LOG(string("Begin Setting current gui = ") + guiName);
		if(_guis.count(guiName) <= 0)
		{
			ADD_LOG("GUI not found");
			return RETURN_ERROR(ErrorCodes::GuiNotFound);
		}

		if(_currentGui.IsValid())
		{
			_currentGui->HideAllLayouts();
		}

		_currentGui = _guis.find(guiName)->second;
		_currentGui->ShowAllLayouts();
		ADD_LOG("End Setting current gui");
		return ErrorCodes::Success;
	}

	GUIManager::~GUIManager()
	{
		for(auto guiIt = _guis.begin(), end = _guis.end(); guiIt != end; ++guiIt)
		{
			(*guiIt).second->UnloadAllLayouts();
		}
		_guis.clear();
		_currentGui = RefPtr<GUI>::InvalidRefPointer();

		if(_mygui != 0)
		{
			_mygui->shutdown();
			delete _mygui;
			_mygui = 0;
		}
		if(_ogre != 0)
		{
			_ogre->shutdown();
			delete _ogre;
			_ogre = 0;
		}
	}

	void GUIManager::Update(float ms)
	{
		// Update keyboard input -> check all keys
		for(int k = 0; k < 256; k++)
		{
			if(InputManager::Instance()->CheckKeyWasPressed(k))
			{
				unsigned char c = 0;
				auto keyCharIt = SystemSettings::GetKeyBindings().KeyCodeToASCIIMap.find(k);
				if( keyCharIt != SystemSettings::GetKeyBindings().KeyCodeToASCIIMap.end())
					c = keyCharIt->second;
				MyGUI::InputManager::getInstance().injectKeyPress((MyGUI::KeyCode::Enum)k, c);
			}
			else if(InputManager::Instance()->CheckKeyWasReleased(k))
			{
				MyGUI::InputManager::getInstance().injectKeyRelease((MyGUI::KeyCode::Enum)k);
			}
		}
		// Update mouse input -> check move then all buttons
		Vector2 mpos = InputManager::Instance()->GetMouseScreenPosAbs();
		float scroll = InputManager::Instance()->GetInputState()->GetScrollChange();

		if(	InputManager::Instance()->CheckMouseMovedSinceLastFrame())
		{
			MyGUI::InputManager::getInstance()
				.injectMouseMove((int)mpos.x, (int)mpos.y, (int)scroll);
		}
		for(int k = 0; k < 3; k++)
		{
			if(InputManager::Instance()->CheckMouseWasPressed(k))
			{
				MyGUI::InputManager::getInstance()
					.injectMousePress((int)mpos.x, (int)mpos.y, (MyGUI::MouseButton::Enum)k);
			}
			else if(InputManager::Instance()->CheckMouseWasReleased(k))
			{
				MyGUI::InputManager::getInstance()
					.injectMouseRelease((int)mpos.x, (int)mpos.y, (MyGUI::MouseButton::Enum)k);
			}
			
		}

		if( _currentGui.IsValid() )
			_currentGui->Update(ms);
	}
}