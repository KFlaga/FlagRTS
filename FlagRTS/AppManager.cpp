#include "AppManager.h"
#include <Memory.h>
#include <Exception.h>
#include <Managers.h>
#include "AppGlobalState.h"
#include "IntroAppState.h"
#include "MainMenuState.h"
#include "GameLoadingState.h"
#include "InGameState.h"
#include <Windows.h>
#include "Constants.h"
#include <Clock.h>
#include <StdAllocator.h>

#include <SystemSettings.h>
#include <GameSettings.h>
#include <Profiler.h>

namespace FlagRTS
{
	AppManager* AppManager::_instance = 0;

	void QuickEnd(AppManager* appManager);

	AppManager::AppManager() :
		_currentState(0), _changeStatePending(false)
	{
		LogManager::Initialize();
		ErrorManager::Initialize();
		ConfigurationManager::Initialize();
		ThreadManager::Initialize();
		RenderSystemManager::Initialize();
		InputManager::Initialize();
		GUIManager::Initialize();
		CursorManager::Initialize();
		LocalizationManager::Initialize();
		Media::AudioManager::Initialize();
	}

	AppManager::~AppManager()
	{
		ADD_LOG("Destroying AppManger");

		Media::AudioManager::Release();
		LocalizationManager::Release();
		CursorManager::Release();
		GUIManager::Release();
		InputManager::Release();
		RenderSystemManager::Release();
		ThreadManager::Release();
		ConfigurationManager::Release();
		ErrorManager::Release();

		auto mem = (StdAllocator*)Malloc;

		// Get final statistics on memory
		ADD_MEMLOG(string("Leftovers :"));
		ADD_MEMLOG(string("Allocated items : ") + std::to_string(mem->GetAllocatedItems()));
		ADD_MEMLOG(string("Allocated memory : ") + std::to_string(mem->GetAllocatedMemory()));

		LogManager::Release();
	}

	bool AppManager::Run()
	{
		LogManager::Instance()->OpenLog(LogManager::GeneralLog, "log.log",true);
		LogManager::Instance()->OpenLog(LogManager::MemoryLog, "memory.log",true);
		LogManager::Instance()->OpenLog(LogManager::ErrorLog, "error.log",true);
		ADD_LOG("APP_RUN");
		_runTime = 0.f;
		CreateQuickEndThread();
		LoadConfigs();

		RenderSystemManager::Instance()->InitOgre();
		InitBasicResources();
		InputManager::Instance()->InitOIS(RenderSystemManager::Instance()->GetMainWindow());
		InitAppStates();

		Clock updateClock;
		updateClock.Start();
		Clock fpsLimiter;
		fpsLimiter.Start();

		PROFILE_REGISTER_CLOCK(loopClock, "GameLoop");
		PROFILE_REGISTER_CLOCK(renderClock, "RenderLoop");
		PROFILE_REGISTER_CLOCK(mainClock, "MainLoop");
		PROFILE_REGISTER_CLOCK(loopClockTotal, "GameLoopTotal");
		PROFILE_REGISTER_CLOCK(renderClockTotal, "RenderLoopTotal");
		while(true)
		{
			PROFILE_START_NEW(mainClock);
			fpsLimiter.Reset();
			// Change state if requested
			if(_changeStatePending)
			{
				NextStateInternal();
				if(_currentState == 0)
					return false;
			}

			float interval = updateClock.GetMilisecSinceLastCheckR();
			_runTime += interval;

			// Process window messages
			Ogre::WindowEventUtilities::messagePump();

			PROFILE_RESET_CLOCK(loopClock);
			PROFILE_START(loopClock);
			PROFILE_START(loopClockTotal);
			// Update logic of current state
			if(_currentState->Update(interval) != ErrorCodes::Success)
				break;
			PROFILE_END(loopClock);
			PROFILE_END(loopClockTotal);

			PROFILE_RESET_CLOCK(renderClock);
			PROFILE_START(renderClock);
			PROFILE_START(renderClockTotal);
			// Render current frame
			if(!RenderSystemManager::Instance()->Render())
				break;
			PROFILE_END(renderClock);
			PROFILE_END(renderClockTotal);


			//RenderSystemManager::Instance()->Render();
			// Process tasks - one per frame
			if(_tasks.size() > 0)
			{
				_tasks.front()->operator()();
				_tasks.pop_front();
			}

			PROFILE_END(mainClock);
			PROFILE_LOG_TO_FILE("profiler.log");

			float maxFps = (float)SystemSettings::GetGameSettings().MaxFPS;
			if( SystemSettings::GetGameSettings().MaxFPS > 0.0f )
			{
				int waitus = (int)1000.f*(1000.0f/maxFps - fpsLimiter.GetMilisecSinceStartR());
				std::this_thread::sleep_for(std::chrono::microseconds(waitus));
			}
		}

		UnloadAllStates();
		ThreadManager::Instance()->GetThread("QuickEndThread")->End();
		return true;
	}

	void AppManager::InitAppStates()
	{
		IntroAppState* introState(xNew0(IntroAppState));
		_availableStates.insert(std::make_pair("Intro", introState));
		MainMenuState* mainMenuState(xNew0(MainMenuState));
		_availableStates.insert(std::make_pair("MainMenu", mainMenuState));
		GameLoadingState* gameLoadState(xNew0(GameLoadingState));
		_availableStates.insert(std::make_pair("GameLoading", gameLoadState));
		InGameState* ingameState(xNew0(InGameState));
		_availableStates.insert(std::make_pair("InGame", ingameState));
		PushState("Intro");
		NextState();
	}

	void AppManager::LoadConfigs()
	{
		ADD_LOG("Loading Configs");

		RefPtr<Configuration> mainConf = ConfigurationManager::Instance()->GetConfiguration("Main");
		if( (*mainConf).Load(Constants::configFilePath) != ErrorCodes::Success )
		{
			ADD_LOG( (string("Failed to load : ") + Constants::configFilePath + string(". Loading defaults")) );
			SetDefaultMainConfig(mainConf);
		}

		string settingsPath = mainConf->FindOption("SettingsConfig", "Files")->Value;
		RefPtr<Configuration> settingsConf = ConfigurationManager::Instance()->GetConfiguration("Settings");
		if( settingsConf->Load(settingsPath) != ErrorCodes::Success )
		{
			ADD_LOG( (string("Failed to load : ") + settingsPath + string(". Loading defaults")) );
			SetDefaultSettingsConfig(settingsConf);
		}

		string resConfigPath = mainConf->FindOption("ResourcesConfig", "Files")->Value;
		RefPtr<Configuration> resConf = ConfigurationManager::Instance()->GetConfiguration("Resources");
		if( resConf->Load(resConfigPath) != ErrorCodes::Success)
		{
			CastException("Failed to load resources config");
		}

		string guiConfPath = mainConf->FindOption("GuiConfig", "Files")->Value;
		RefPtr<Configuration> guiConf = ConfigurationManager::Instance()->GetConfiguration("Gui");
		if( guiConf->Load(guiConfPath) != ErrorCodes::Success)
		{
			CastException("Failed to load gui config");
		}

		SystemSettings::LoadAllSettingsFromConfigFile("Settings");

		ADD_LOG("End Loading Configs");
	}

	void AppManager::InitBasicResources()
	{
		RefPtr<Configuration> resConf = ConfigurationManager::Instance()->
			GetConfiguration("Resources");

		// Find base resource loacations in resources config
		// Section with resource group in format : ResourceGroup.GroupName
		auto resIt = resConf->BeginDict();
		for(; resIt != resConf->EndDict(); resIt++)
		{
			// For each section find if it is resource group
			SectionIterator resGroupIt = (*resIt).Value->begin();
			int pos = (*resGroupIt).Value->Section.find("ResourceGroup");
			if(pos != string::npos) // Found substr, so it is resource group
			{
				// Find name of group -> after firt dot
				pos = (*resGroupIt).Value->Section.find('.');
				string groupName = (*resGroupIt).Value->Section.substr(pos+1);
				// Add all resource locations from this group and initialize
				for(; resGroupIt != (*resIt).Value->end(); resGroupIt++)
				{
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
						(*resGroupIt).Value->Value, (*resGroupIt).Value->Name, groupName);
				}
			}
		}
		Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("MyGUI");
	}

	IAppState* AppManager::GetState(const string& name)
	{
		if(_availableStates.count(name) == 0)
			return 0;
		return _availableStates.find(name)->second;
	}

	ErrorCode AppManager::PushState(const string& name)
	{
		if(_availableStates.count(name) == 0)
			return ErrorCodes::AppStateNameNotFound;

		IAppState* state = _availableStates.find(name)->second;
		this->_stateQueue.push_back(state);
		state->Load();
		return ErrorCodes::Success;
	}

	void AppManager::NextState()
	{
		_changeStatePending = true;
	}

	void AppManager::NextStateInternal()
	{
		_changeStatePending = false;
		if(_currentState != 0)
		{
			_currentState->End();
			_currentState = 0;
		}
		if( _stateQueue.size() > 0 )
		{
			_currentState = _stateQueue.front();
			_stateQueue.pop_front();
			_currentState->Start();
		}
	}

	ErrorCode AppManager::RemoveState(const string& name)
	{
		if(_availableStates.count(name) == 0)
			return ErrorCodes::AppStateNameNotFound;

		// try on  unload??
		IAppState* state = _availableStates.find(name)->second;
		_stateQueue.remove(state);
		state->Unload();

		return ErrorCodes::Success;
	}

	void AppManager::SheduleTask(RefPtr<Task> task)
	{
		_tasks.push_back(task);
	}

	void AppManager::UnloadAllStates()
	{
		for(auto it = _availableStates.begin(); it != _availableStates.end(); it++)
		{
			it->second->Unload();
			xDelete(it->second);
		}
		_availableStates.clear();
	}

	void AppManager::SetDefaultMainConfig(const RefPtr<Configuration>& conf)
	{
#ifdef _DEBUG
		conf->AddOption("Plugin_RenderSystem_Direct3D9", "RenderSystem_Direct3D9_d", "Plugins.Ogre");
		conf->AddOption("Plugin_ParticleFX", "Plugin_ParticleFX", "Plugins.Ogre");
#else
		conf->AddOption("Plugin_RenderSystem_Direct3D9", "RenderSystem_Direct3D9", "Plugins.Ogre");
		conf->AddOption("Plugin_ParticleFX", "Plugin_ParticleFX_d", "Plugins.Ogre");
#endif
		conf->AddOption("Resources", "../../resources/", "Directories");
		conf->AddOption("Maps", "../../resources/Maps/", "Directories");
		conf->AddOption("Mods", "../../resources/Mods/", "Directories");
		conf->AddOption("ModsDll", "mods/", "Directories");
		conf->AddOption("Languages", "lng/", "Directories");

		conf->AddOption("SettingsConfig", "settings.cfg", "Files");
		conf->AddOption("ResourcesConfig", "resources.cfg", "Files");
		conf->AddOption("GuiConfig", "../../resources/Gui/gui.cfg", "Files");

		conf->Save( Constants::configFilePath );
	}

	void AppManager::SetDefaultSettingsConfig(const RefPtr<Configuration>& conf)
	{
		conf->AddOption("RenderSystem", "Direct3D9 Rendering Subsystem", "Video.RenderSystem");
		conf->AddOption("Video Mode", "800 x 600 @ 32-bit colour", "Video.Settings");
		conf->AddOption("Full Screen", "No", "Video.Settings");
		conf->AddOption("FSAA", "0", "Video.Settings");
		conf->AddOption("VSync", "0", "Video.Settings");

		RefPtr<Configuration> mainConf = ConfigurationManager::Instance()->GetConfiguration("Main");
		string settingsPath = mainConf->FindOption("SettingsConfig", "Files")->Value;
		conf->Save( settingsPath );
	}

	void AppManager::CreateQuickEndThread()
	{
		// Creates thread that allows for Alt+F4 app closing during block
		Thread* quickEndThread = ThreadManager::Instance()->CreateThread("QuickEndThread");
		if(!Thread::IsMainThread())
		{
			Thread* currThread = ThreadManager::Instance()->GetCurrentThread();
			currThread->AddChildThread(quickEndThread);
		}
		quickEndThread->Run(QuickEnd, this);
	}

	void QuickEnd(AppManager* appManager)
	{
		while(ThreadManager::Instance()->GetThread("QuickEndThread")->ShouldEnd() == false)
		{
			auto keyAlt = GetAsyncKeyState(VK_MENU);
			auto keyF4 = GetAsyncKeyState(VK_F4);
			if( (keyF4 & 0x8000) != 0 && (keyAlt & 0x8000) != 0)
			{
				abort();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

}