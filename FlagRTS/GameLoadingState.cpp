#include "GameLoadingState.h"
#include "AppManager.h"
#include "Constants.h"
#include "LoadingScreen.h"
#include <Managers.h>
#include <Memory.>
#include <Ogre\Ogre.h>
#include <OIS\OISKeyboard.h>
#include "InGameState.h"
#include <InGameSettings.h>
#include <GameController.h>

namespace FlagRTS
{
	GameLoadingState::GameLoadingState() :
		_ogre(RenderSystemManager::Instance()->GetOgreRoot()),
		_mainWindow(RenderSystemManager::Instance()->GetMainWindow()),
		_loadingScreen(0)
	{

	}

	GameLoadingState::~GameLoadingState()
	{
		xDeleteSafe(_loadingScreen);
	}

	ErrorCode GameLoadingState::Load()
	{
		ADD_LOG("Begin Loading GameLoadingState");
		// Create ogre objects and loading screen
		_sceneManager = _ogre->createSceneManager(Ogre::ST_GENERIC, "GameLoadingStateSceneManager");
		
		_camera = _sceneManager->createCamera("GameLoadingStateCamera");
		_camera->setNearClipDistance(1);
		_mainWindow->removeAllViewports();
		Ogre::Viewport* vp = _mainWindow->addViewport(_camera);
		vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
		_camera->setAspectRatio(vp->getWidth() / vp->getHeight());
		_loadingScreen = xNew0(LoadingScreen);

		ADD_LOG("End Loading GameLoadingState");
		return ErrorCodes::Success;
	}

	ErrorCode GameLoadingState::Unload()
	{
		if(_loadingScreen)
		{
			_loadingScreen->Unload();
		}
		//_sceneManager->destroyAllCameras(); // ogre should handle it
		//_ogre->destroySceneManager(_sceneManager);

		return ErrorCodes::Success;
	}

	ErrorCode GameLoadingState::Start()
	{
		ADD_LOG("Begin Start: GameLoadingState");

		_loadingScreen->Show();
		// TODO add asynchronus map loading
		_gameController = xNew3(GameController,_ogre, _mainWindow, _gameSettings);
		_gameController->LoadMap();
		InGameState* gameState = (InGameState*)(AppManager::Instance()->GetState("InGame"));
		if(gameState != 0)
		{
			gameState->SetGameController(_gameController);
			AppManager::Instance()->PushState("InGame");
			AppManager::Instance()->NextState();
		}

		ADD_LOG("End Start: GameLoadingState");
		return ErrorCodes::Success;
	}

	ErrorCode GameLoadingState::End()
	{
		Unload();
		return ErrorCodes::Success;
	}

	ErrorCode GameLoadingState::Update(float ms)
	{
		// It shouldn't reach here for now
		
		// update input and gui
		InputManager::Instance()->Update(ms);
		// If esc is pressed - end app
		if(InputManager::Instance()->CheckKeyIsDown(OIS::KC_DELETE))
		{
			return ErrorCodes::Exit;
		}

		GUIManager::Instance()->Update(ms);

		return ErrorCodes::Success;
	}

	void GameLoadingState::SetGameSettings(InGameSettings* gameSettings)
	{
		_gameSettings = gameSettings;
	}
}