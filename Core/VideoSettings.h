#pragma once

#include "TypeDefs.h"
#include "Configuration.h"

namespace FlagRTS
{
	class VideoSettings
	{
	public:
		string ConfigName;
		//std::map<string, int> DetaliLevels;

		// Render System settings
		string RenderSystem;
		string VideoMode;
		bool IsFullscreen;
		bool IsVSync;
		int FSAA;

		// float ViewDistance;
		// float ActorFadeDistance;
		// float NatureFadeDistance;
		// float HighLevelDistance;

		// Internal video settings
		//int TextureQuality;
		//int ModelQuality;
		//int EffectQuality;
		//etc etc
		
		VideoSettings();

		// Loads settings from config with given name
		void LoadSettingsFromConfig(const string& configName);
		// Saves config to config it was loaded from
		void SaveSettingsToConfig();
		// Saves config to config it  with given name
		void SaveSettingsToConfig(const string& configName);
	};
}