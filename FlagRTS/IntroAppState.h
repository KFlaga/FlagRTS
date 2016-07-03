#pragma once

#include "AppGlobalState.h"
#include <Pointers.h>
#include "Task.h"

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

	// First app state which begin on application initialization
	// Requests loading of menu
	//
	class IntroAppState : public IAppState
	{
	private:
		Ogre::Root* _ogre;
		Ogre::RenderWindow* _mainWindow;
		Ogre::SceneManager* _sceneManager;
		Ogre::Camera* _camera;
		LoadingScreen* _loadingScreen;

	public:
		IntroAppState();
		~IntroAppState();

		ErrorCode Load();
		ErrorCode Unload();
		ErrorCode Start();
		ErrorCode End();
		ErrorCode Update(float ms);

	protected:
		// Load tasks
		class TaskInitResources : public Task
		{
		private:
			IntroAppState* _state;
		public:
			TaskInitResources(IntroAppState* state) :
				Task(), _state(state) { }

			void operator()()
			{
				_state->InitResources();
			}
		};
		void InitResources();


		class TaskLoadLocale : public Task
		{
		private:
			IntroAppState* _state;
		public:
			TaskLoadLocale(IntroAppState* state) :
				Task(), _state(state) { }

			void operator()()
			{
				_state->LoadLocale();
			}
		};
		void LoadLocale();

		class TaskLoadSounds : public Task
		{
		private:
			IntroAppState* _state;
		public:
			TaskLoadSounds(IntroAppState* state) :
				Task(), _state(state) { }

			void operator()()
			{
				_state->LoadSounds();
			}
		};
		void LoadSounds();

		class TaskPushMainMenu : public Task
		{
		private:
			IntroAppState* _state;
		public:
			TaskPushMainMenu(IntroAppState* state) :
				Task(), _state(state) { }

			void operator()()
			{
				_state->PushMainMenu();
			}
		};
		void PushMainMenu();
	};
}

