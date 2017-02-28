#pragma once

#include <GenericManager.h>
#include <List.h>
#include <KeyValMap.h>
#include <Pointers.h>
#include <TypeDefs.h>
#include <OGRE\Ogre.h>
#include <StringUtils.h>
#include "Task.h"

namespace FlagRTS
{
	class Configuration;
	class IAppState;

	class AppManager : public GenericManager<AppManager>
	{
	private:
		typedef std::map<string, IAppState*, StringLess> StateMap;
		typedef List<IAppState*> StateQueue;
		typedef List<IAppState*>::iterator StateIterator;
		typedef List<RefPtr<Task>> TaskList;

		StateQueue _stateQueue;
		IAppState* _currentState;
		StateMap _availableStates; // Map constains all global app states with string keys
		bool _changeStatePending;
		TaskList _tasks;

		float _runTime;

	protected:
		AppManager();
		friend GenericManager<AppManager>;

	private:
		// Loads main configuration files
		void LoadConfigs();
		// Sets default values for main config ( if unable to read file ) or resources config
		void SetDefaultMainConfig(const RefPtr<Configuration>& config);
		void SetDefaultSettingsConfig(const RefPtr<Configuration>& config);
		// Add resources locations and initializes them ( only non-ingame resources ) ( locations read from resconfig)
		void InitBasicResources();
		// Creates all available state of app ( do not load them ) and names them
		// Pushes IntroState
		void InitAppStates();

		// Changes state if pending flag is set
		// Calls old state's End() and new one's Start()
		void NextStateInternal();
		
		// Unload all initialized states
		void UnloadAllStates();

		// Get total time since call to Run()
		float GetRunTime() const { return _runTime; }
		void CreateQuickEndThread();

	public:
		bool Run();

		// Returns state with given name, 0 if not found
		IAppState* GetState(const string& name);
		// Pushes state to queue and load it
		ErrorCode PushState(const string& name);
		// Set flag to change state to next from queue on next update
		void NextState();
		// Removes state from queue and unloads it
		ErrorCode RemoveState(const string& name);

		void SheduleTask(RefPtr<Task> task);

		~AppManager();
	};
}