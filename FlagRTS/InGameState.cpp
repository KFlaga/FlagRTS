#include "InGameState.h"
#include <Managers.h>
#include <ErrorCodes.h>
#include "AppManager.h"
#include <InGameSettings.h>
#include <OISKeyboard.h>
#include <Memory.h>
#include <GameController.h>
#include <AudioManager.h>


namespace FlagRTS
{
	InGameState::InGameState() :
		_ogre(RenderSystemManager::Instance()->GetOgreRoot()), 
		_mainWindow(RenderSystemManager::Instance()->GetMainWindow()),
		_game(0)
	{

	}

	InGameState::~InGameState()
	{
		xDeleteSafe(_game);
	}

	ErrorCode InGameState::Load()
	{
		Media::AudioManager::Instance()->LoadSoundPack("GameDefaults");

		return ErrorCodes::Success;
	}

	ErrorCode InGameState::Unload()
	{
		if(_game)
		{
			_game->UnloadMap();
			xDelete(_game);
			_game = 0;
		}
		return ErrorCodes::Success;
	}

	ErrorCode InGameState::Start()
	{
		// Here all is loaded for sure, so thread can be removed

		_game->BeginGame();

		return ErrorCodes::Success;
	}

	ErrorCode InGameState::End()
	{

		return ErrorCodes::Success;
	}

	ErrorCode InGameState::Update(float ms)
	{
		// update input and gui
		InputManager::Instance()->Update(ms);
		// If delete is pressed - back to main menu
		if(InputManager::Instance()->CheckKeyIsDown(OIS::KC_ESCAPE))
		{
			AppManager::Instance()->PushState("MainMenu");
			AppManager::Instance()->NextState();
			return ErrorCodes::Success;
		}

		Media::AudioManager::Instance()->Update(ms);
		GUIManager::Instance()->Update(ms);

		ErrorCode ec = _game->Update(ms);

		return ec;
	}
}