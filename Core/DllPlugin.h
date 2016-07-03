#pragma once

#include "TypeDefs.h"
#include "Memory.h"

namespace FlagRTS
{
	// Base class for dll plugins
	// Every plugin needs to contain a function named "StartPlugin" and "EndPlugin"
	// with type of DllPlugin::PluginFunc defined in some cpp in similar way:
	// 
	// SomeDerivedPluginClass* plugin = 0;
	// extern "C" DllPlugin* DllApi StartPlugin(Allocator* alloc)
	// {
	//		SET GLOBAL MALLOC = alloc.GetAllocator();
	//		if(plugin == 0) 
	//			plugin = xNew(SomeDerivedPluginClass);
	//		return plugin;
	// }
	// 
	// extern "C" DllPlugin* DllApi EndPlugin(void)
	// {
	//		xDelete(plugin)
	//		plugin = 0;
	//		FREE ALLOCATOR
	// }
	//
	// 

	// Contain all global object instances from Core passed to dlls
	struct TypeHelper;
	namespace Media
	{
		class AudioManager;
	}
	class ConfigurationManager;
	class GUIManager;
	class InputManager;
	class LocalizationManager;
	class LogManager;
	class ThreadManager;
	class RenderSystemManager;
	struct DllPluginGlobalInstances
	{
		Allocator* Allocator;
		TypeHelper* TypeHelper;
		Media::AudioManager* AudioManager;
		ConfigurationManager* ConfigurationManager;
		GUIManager* GUIManager;
		InputManager* InputManager;
		LocalizationManager* LocalizationManager;
		LogManager* LogManager;
		ThreadManager* ThreadManager;
		RenderSystemManager* RenderSystemManager;

		DllPluginGlobalInstances();
	};

	class DllApi DllPlugin
	{
	public:
		typedef DllPlugin*(*PluginStartFuncType)(DllPluginGlobalInstances*);
		typedef void(*PluginEndFuncType)();
		// Loads given dll and calls StartPlugin from it. Returns retrieved plugin
		static DllPlugin* GetPlugin(const char* pluginPath, DllPluginGlobalInstances* pluginMalloc);
		// Class EndPlugin from plugin's dll and frees the library
		static void ReleasePlugin(DllPlugin* plugin);

	protected:
		const char* _path;
		size_t _dllHandle;

	public:
		DllPlugin() :
			_dllHandle(0)
		{ }

		virtual ~DllPlugin() { }

		virtual void Init() { }
		virtual void Deinit() { }

		void SetPathToDll(const char* path) { _path = path; }
		const char* GetPathToDll() const { return _path; }

		size_t GetPluginDllHandle() const { return _dllHandle; }
		void SetPluginDllHandle(size_t handle) { _dllHandle = handle; }
	};

	void SetGlobalInstances(DllPluginGlobalInstances*);
}