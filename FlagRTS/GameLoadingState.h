#pragma once

#include "AppGlobalState.h"
#include <Pointers.h>

namespace Ogre
{
	class Root;
	class RenderWindow;
	class SceneManager;
	class Camera;
}


namespace FlagRTS
{
	class LoadingScreen;
	struct InGameSettings;
	class GameController;

	// In this state loading of GameState is requested
	// and loading screen shown untill GameWorld is fully loaded
	class GameLoadingState : public IAppState
	{
	private:
		Ogre::Root* _ogre;
		Ogre::RenderWindow* _mainWindow;
		Ogre::SceneManager* _sceneManager;
		Ogre::Camera* _camera;
		LoadingScreen* _loadingScreen;
		InGameSettings* _gameSettings;
		GameController* _gameController;

	public:
		GameLoadingState();
		~GameLoadingState();

		void SetGameSettings(InGameSettings* gameSettings);

		// Prior to Load, InGameSettings should be set
		ErrorCode Load();
		ErrorCode Unload();
		ErrorCode Start();
		ErrorCode End();
		ErrorCode Update(float ms);

	};

}