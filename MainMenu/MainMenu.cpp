#include "MainMenu.h"
#include <GUIManager.h>
#include <LogManager.h>
#include <LocalizationManager.h>
#include <ErrorCodes.h>
#include <LogManager.h>
#include <RenderSystemManager.h>

#include "OptionsBase.h"
#include "MainScreen.h"
#include "CreateGameScreen.h"

namespace FlagRTS
{
	MainMenu::MainMenu() : 
		_currentScreen(0), 
		_startGamePending(false),
		_onChangeScreenRequested(this),
		_changeScreenPending(false),
		_onNewGameRequested(this),
		_resizeGui(this)
	{
		ADD_LOG("Begin Creating MainMenu");
		
		MainScreen* mainScreen = xNew0(MainScreen);
		mainScreen->GetChangeScreenRequestedEvent() += &_onChangeScreenRequested;
		OptionsBase* optionsScreen = xNew0(OptionsBase);
		optionsScreen->GetChangeScreenRequestedEvent() += &_onChangeScreenRequested;
		CreateGameScreen* createGameScreen = xNew0(CreateGameScreen);
		createGameScreen->GetChangeScreenRequestedEvent() += &_onChangeScreenRequested;
		createGameScreen->GetNewGameStartRequestedEvent() += &_onNewGameRequested;

		_menuScreens.push_back(mainScreen);
		_menuScreens.push_back(optionsScreen);
		_menuScreens.push_back(createGameScreen);

		RenderSystemManager::Instance()->WindowResized() += &_resizeGui;

		SetCurrentScreen("MainScreen");

		ADD_LOG("End Creating MainMenu");
	}

	MainMenu::~MainMenu()
	{
		Unload();
	}

	ErrorCode MainMenu::Update(float ms)
	{
		if(_currentScreen != 0)
		{
			// Handle one request each frame
			if(_changeScreenPending)
			{
				SetCurrentScreen(_changeScreenName);
				_changeScreenPending = false;
			}

			ErrorCode ec = _currentScreen->Update(ms);
			return ec;
		}
		return ErrorCodes::Success;
	}

	void MainMenu::Show()
	{
		if(_currentScreen != 0)
		{
			_currentScreen->Show();
		}
	}

	void MainMenu::Hide()
	{
		if(_currentScreen != 0)
		{
			_currentScreen->Hide();
		}
	}

	void MainMenu::Unload()
	{
		// Unload all screens
		for(unsigned int i = 0; i < _menuScreens.size(); ++i)
		{
			_menuScreens[i]->Unload();
			xDelete(_menuScreens[i]);
		}
		_menuScreens.clear();
	}

	void MainMenu::SetCurrentScreen(const string& name)
	{
		for(unsigned int i = 0; i < _menuScreens.size(); ++i)
		{
			if(_menuScreens[i]->GetName().compare(name) == 0)
			{
				if(_currentScreen != 0)
				{
					_currentScreen->Hide();
				}
				_currentScreen = _menuScreens[i];
				_currentScreen->Show();
			}
		}
	}

	void MainMenu::ChangeScreenRequested(string screenName)
	{
		_changeScreenPending = true;
		_changeScreenName = screenName;
	}

	void MainMenu::NewGameRequested(NewGameSettings* gameSettings)
	{
		_newGameRequested.Fire(gameSettings);
	}

	void MainMenu::ResizeGui(std::pair<int,int> size)
	{
		for(unsigned int i = 0; i < _menuScreens.size(); ++i)
		{
			_menuScreens[i]->Resize(size.first, size.second);
		}
	}
}