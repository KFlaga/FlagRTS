#pragma once

#include "TypeDefs.h"

namespace FlagRTS
{
	class VideoSettings;
	class AudioSettings;
	class CameraSettings;
	class InputSettings;
	class GameSettings;
	class KeyBindings;

	// Global settings used to customize game, that is :
	// - video settings ( like resolution, details )
	// - game settings ( like game speed, difficulty )
	// - camera settings ( like camera move speed )
	// - audio settings ( volumes )
	// - input settings ( like mouse sensitivity )
	namespace SystemSettings
	{
		// Global accessors to specialized settings
		// Accessed this way to minimize compilation time dependencies
		// ( source need to include this .h and one of settings )
		 VideoSettings& GetVideoSettings();
		 AudioSettings& GetAudioSettings();
		 CameraSettings& GetCameraSettings();
		 InputSettings& GetInputSettings();
		 GameSettings& GetGameSettings();
		 KeyBindings& GetKeyBindings();
		 // Loads settings from config file
		 void LoadAllSettingsFromConfigFile(const string& configName);
	};
}