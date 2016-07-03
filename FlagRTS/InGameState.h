#pragma once

#include "AppGlobalState.h"
#include <Pointers.h>
#include <OGRE\Ogre.h>

namespace Ogre
{
	class Root;
	class RenderWindow;
	class SceneManager;
	class Camera;
}

namespace FlagRTS
{
	class GameController;

	class InGameState : public IAppState
	{
	private:
		Ogre::Root* _ogre;
		Ogre::RenderWindow* _mainWindow;
		Ogre::SceneManager* _sceneManager;
		Ogre::Camera* _camera;
		GameController* _game;

	public:
		InGameState();
		~InGameState();

		void SetGameController(GameController* game)
		{
			_game = game;
		}

		ErrorCode Load();
		ErrorCode Unload();
		// Loaded game controller must be set before start
		ErrorCode Start();
		ErrorCode End();
		ErrorCode Update(float ms);
	};

}