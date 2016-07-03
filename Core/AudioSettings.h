#pragma once

#include "TypeDefs.h"
#include "Pointers.h"
#include "Configuration.h"

namespace FlagRTS
{
	class AudioSettings
	{
	public:
		string ConfigName;

		float MasterVolume; // How load all sounds are. 1.0 means max volume
		float GUIVolume; // Buttons clicks  etc
		float MusicVolume;
		float AlertsVolume; // Ingame notices
		float VoiceVolume; // Voulme of dialogs in cutscenes etc
		float ResponseVolume; // Voulme of unit commads responses
		float GameVolme; // Voulme of in-game sounds ( weapon fires / units movement / ambient )

		AudioSettings();
		
		// Loads settings from config with given name
		void LoadSettingsFromConfig(const string& configName);
		// Saves config to config it was loaded from
		void SaveSettingsToConfig();
		// Saves config to config it  with given name
		void SaveSettingsToConfig(const string& configName);
	};
}