#pragma once

#include "AppGlobalState.h"
#include <Pointers.h>
#include "Task.h"
#include <Event.h>

namespace Ogre
{
	class Root;
	class RenderWindow;
	class SceneManager;
	class Camera;
}

namespace FlagRTS
{
	class MainMenu;
	struct NewGameSettings;
	
	namespace Media
	{
		class SoundSource;
		class ISoundPlayer;
	}

	// First app state which begin on application initialization
	// Requests loading of intro, then menu
	//
	class MainMenuState : public IAppState
	{
	private:
		Ogre::Root* _ogre;
		Ogre::RenderWindow* _mainWindow;
		Ogre::SceneManager* _sceneManager;
		Ogre::Camera* _camera;
		string _loadingStatus;
		MainMenu* _menuGui;

		Media::ISoundPlayer* _playerButtonClickOk;
		Media::SoundSource* _soundButtonClickOk;
		Media::ISoundPlayer* _playerButtonClickCancel;
		Media::SoundSource* _soundButtonClickCancel;

	public:
		MainMenuState();
		~MainMenuState();

		ErrorCode Load();
		ErrorCode Unload();
		ErrorCode Start();
		ErrorCode End();
		ErrorCode Update(float ms);

		const string& GetLoadingStatus() const { return _loadingStatus; }

	private:
		class TaskBuildMenu : public Task
		{
		private:
			MainMenuState* _mmstate;
		public:
			TaskBuildMenu(MainMenuState* mmstate) :
				Task(), _mmstate(mmstate) { }

			void operator()()
			{
				_mmstate->BuildMainMenu();
			}
		};

		void BuildMainMenu();

		void NewGameRequested(NewGameSettings* gameSettings);
		DEFINE_DELEGATE1(NewGameRequestedDelegate, MainMenuState, 
			NewGameRequested, NewGameSettings*);
		NewGameRequestedDelegate _onNewGameRequested;
	};

}