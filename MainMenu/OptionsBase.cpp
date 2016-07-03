#include "OptionsBase.h"
#include <GUIManager.h>
#include <LogManager.h>
#include <ConfigurationManager.h>
#include <ErrorManager.h>
#include <CursorManager.h>
#include <LocalizationManager.h>
#include "MainMenu.h"
#include "OptionsVideo.h"

namespace FlagRTS
{
	OptionsBase::OptionsBase() :
		IMenuScreen(xNew1(GUI,"Options"), "Options"),
		_bgPanel(0),
		_buttonVideo(0),
		_buttonAudio(0),
		_buttonInput(0),
		_buttonGame(0),
		_buttonReturn(0),
		_loaded(false),
		_currentOptionsPanel(0)
	{
		ADD_LOG("Begin Create OptionsBase");
		// InitGui
		if( _gui->LoadLayout("OptionsBase.layout", 0, true) != ErrorCodes::Success)
		{
			ADD_LOG("Couldn't load layout : OptionsBase.layout");
		}

		// Assign widgets
		RefPtr<Layout> mainLayout = _gui->GetLayout("OptionsBase.layout");
		_bgPanel = static_cast<MyGUI::ImageBox*>(_gui->FindInLayout(mainLayout, "OptionsMainPanel"));
		_buttonVideo = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonVideoSettings"));
		_buttonAudio = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonAudioSettings"));
		_buttonInput = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonInputSettings"));
		_buttonGame = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonGameSettings"));
		_buttonReturn = static_cast<MyGUI::Button*>(_gui->FindInLayout(mainLayout, "ButtonReturn"));

		// Set button texts
		_buttonVideo->setCaption(GET_TEXT("ButtonVideoSettings","Gui.MainMenu.Options"));
		_buttonAudio->setCaption(GET_TEXT("ButtonAudioSettings","Gui.MainMenu.Options"));
		_buttonInput->setCaption(GET_TEXT("ButtonInputSettings","Gui.MainMenu.Options"));
		_buttonGame->setCaption(GET_TEXT("ButtonGameSetttings","Gui.MainMenu.Options"));
		_buttonReturn->setCaption(GET_TEXT("ButtonReturn","Gui.MainMenu.Options"));

		// Set event handlers for buttons:
		_buttonVideo->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsBase::ChangeOptionTabToVideo);
		_buttonAudio->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsBase::ChangeOptionTabToAudio);
		_buttonInput->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsBase::ChangeOptionTabToInput);
		_buttonGame->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsBase::ChangeOptionTabToGame);
		_buttonReturn->eventMouseButtonClick += MyGUI::newDelegate(this, &OptionsBase::ReturnToMenu);

		// Add options panels
		_optionsPanels["Video"] = xNew1(OptionsVideo,_gui);

		GUIManager::Instance()->RegisterGUI("Options", _gui);
		_loaded = true;

		ADD_LOG("End Create OptionsBase");
	}

	OptionsBase::~OptionsBase()
	{
		Unload();
	}

	ErrorCode OptionsBase::Update(float ms)
	{
		if( _currentOptionsPanel != 0 )
		{
			_currentOptionsPanel->Update(ms);
		}

		return ErrorCodes::Success;
	}

	void OptionsBase::Unload()
	{
		if(_loaded)
		{
			_loaded = false;
			// Unload all panels
			for(auto it = _optionsPanels.begin(); it != _optionsPanels.end(); it++)
			{
				it->second->Unload();
			}
			_gui->UnloadAllLayouts();

			GUIManager::Instance()->UnregisterGUI("Options"); 
		}
	}

	void OptionsBase::Show()
	{
		// Parse options
		GUIManager::Instance()->SetCurrentGUI("Options");
		ChangeOptionTabToVideo(0);
	}

	void OptionsBase::ChangeOptionTabToVideo(MyGUI::Widget*)
	{
		if(_currentOptionsPanel != 0)
		{
			_currentOptionsPanel->Hide();
		}
		_currentOptionsPanel = _optionsPanels["Video"];
		_currentOptionsPanel->Show();
	}

	void OptionsBase::ChangeOptionTabToAudio(MyGUI::Widget*)
	{
		//	_currentOptionsPanel->Hide();
		//	_optionsPanels["Audio"]->Show();
	}

	void OptionsBase::ChangeOptionTabToInput(MyGUI::Widget*)
	{
		//	_currentOptionsPanel->Hide();
		//	_optionsPanels["Input"]->Show();
	}

	void OptionsBase::ChangeOptionTabToGame(MyGUI::Widget*)
	{
		//	_currentOptionsPanel->Hide();
		//	_optionsPanels["Game"]->Show();
	}

	void OptionsBase::ReturnToMenu(MyGUI::Widget*)
	{
		_changeScreenRequested.Fire("MainScreen");
	}

	void OptionsBase::Resize(int width, int height)
	{

	}
}