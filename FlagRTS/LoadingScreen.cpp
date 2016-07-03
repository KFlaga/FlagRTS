#include "LoadingScreen.h"
#include <LogManager.h>
#include <Memory.h>

namespace FlagRTS
{
	LoadingScreen::LoadingScreen() :
		_gui(xNew1(GUI,("LoadingScreen")))
	{
		ADD_LOG("Creating LoadScreen");
		InitGui();
		GUIManager::Instance()->RegisterGUI("LoadingScreen", _gui); 
	}

	LoadingScreen::~LoadingScreen()
	{
		ADD_LOG("Destroying LoadScreen");
		if(_gui.IsValid())
		{
			GUIManager::Instance()->UnregisterGUI("LoadingScreen");
			_gui->UnloadAllLayouts();
		}
	}

	void LoadingScreen::InitGui()
	{
		(*_gui).LoadLayout("LoadingScreen.layout", 0, true);
		_tbLoading = static_cast<MyGUI::TextBox*>(_gui->FindWidget("TextLoading"));
		_tbStatus = static_cast<MyGUI::TextBox*>(_gui->FindWidget("TextStatus"));
	}

	void LoadingScreen::SetLoadingStatus(const std::string& status)
	{
		_tbStatus->setCaption(status);
	}

	void LoadingScreen::SetLoadingText(const std::string& status)
	{
		_tbLoading->setCaption(status);
	}

	void LoadingScreen::Show()
	{
		GUIManager::Instance()->SetCurrentGUI("LoadingScreen");
	}

	void LoadingScreen::Hide()
	{
		_gui->HideAllLayouts();
	}

	void LoadingScreen::Unload()
	{
		if(_gui.IsValid())
		{
			ADD_LOG("Unloading LoadingScreen");
			_gui->UnloadAllLayouts();
		}
	}
}