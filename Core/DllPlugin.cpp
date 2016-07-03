#include "DllPlugin.h"
#include <Windows.h>
#include "Exception.h"
#include "TypeId.h"

#include "AudioManager.h"
#include "ConfigurationManager.h"
#include "GUIManager.h"
#include "InputManager.h"
#include "LocalizationManager.h"
#include "LogManager.h"
#include "ThreadManager.h"
#include "RenderSystemManager.h"

namespace FlagRTS
{
	DllPlugin* DllPlugin::GetPlugin(const char* pluginPath, DllPluginGlobalInstances* globals)
	{
		HINSTANCE dllHandle = ::LoadLibrary(pluginPath);

		if(dllHandle == 0) 
		{
			// Couldn't load the library
			CastException((string("Bad plugin path: " ) + pluginPath).c_str());
			return 0;
		}

		size_t handle = reinterpret_cast<size_t>(dllHandle);

		PluginStartFuncType startFunc = (PluginStartFuncType)::GetProcAddress(dllHandle, "StartPlugin");
		DllPlugin* plugin = startFunc(globals);
		plugin->SetPathToDll(pluginPath);
		plugin->SetPluginDllHandle(handle);
		return plugin;
	}

	void DllPlugin::ReleasePlugin(DllPlugin* plugin)
	{
		if(plugin != 0 && plugin->GetPluginDllHandle() != 0)
		{
			HINSTANCE dllHandle = reinterpret_cast<HINSTANCE>(plugin->GetPluginDllHandle());
			PluginEndFuncType endFunc = (PluginEndFuncType)::GetProcAddress(dllHandle, "EndPlugin");
			endFunc();
			::FreeLibrary(dllHandle);
			plugin->SetPluginDllHandle(0);
		}
	}

	DllPluginGlobalInstances::DllPluginGlobalInstances()
	{
		this->Allocator = Malloc;
		this->TypeHelper = TypeHelper::GlobalTypeHelper;
		this->AudioManager = Media::AudioManager::Instance();
		this->ConfigurationManager = ConfigurationManager::Instance();
		this->GUIManager = GUIManager::Instance();
		this->InputManager = InputManager::Instance();
		this->LocalizationManager = LocalizationManager::Instance();
		this->LogManager = LogManager::Instance();
		this->ThreadManager = ThreadManager::Instance();
		this->RenderSystemManager = RenderSystemManager::Instance();
	}

	void SetGlobalInstances(DllPluginGlobalInstances* globals)
	{
		Malloc = globals->Allocator->GetAllocator();
		TypeHelper::GlobalTypeHelper = globals->TypeHelper;

		Media::AudioManager::SetNewInstance(globals->AudioManager);
		ConfigurationManager::SetNewInstance(globals->ConfigurationManager);
		GUIManager::SetNewInstance(globals->GUIManager);
		InputManager::SetNewInstance(globals->InputManager);
		LocalizationManager::SetNewInstance(globals->LocalizationManager);
		LogManager::SetNewInstance(globals->LogManager);
		ThreadManager::SetNewInstance(globals->ThreadManager);
		RenderSystemManager::SetNewInstance(globals->RenderSystemManager);
	}
}