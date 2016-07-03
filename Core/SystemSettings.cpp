#include "SystemSettings.h"
#include "VideoSettings.h"
#include "AudioSettings.h"
#include "GameSettings.h"
#include "CameraSettings.h"
#include "InputSettings.h"
#include "KeyBindings.h"
#include "ConfigurationManager.h"

namespace FlagRTS
{
	// Global settings holder - one for app
	struct _SettingsHolder
	{
		VideoSettings Video;
		AudioSettings Audio;
		CameraSettings Camera;
		InputSettings Input;
		GameSettings Game;
		KeyBindings Keys;
	} SettingsHolder;

	VideoSettings& SystemSettings::GetVideoSettings()
	{
		return SettingsHolder.Video;
	}

	AudioSettings& SystemSettings::GetAudioSettings()
	{
		return SettingsHolder.Audio;
	}

	CameraSettings& SystemSettings::GetCameraSettings()
	{
		return SettingsHolder.Camera;
	}

	InputSettings& SystemSettings::GetInputSettings()
	{
		return SettingsHolder.Input;
	}

	GameSettings& SystemSettings::GetGameSettings()
	{
		return SettingsHolder.Game;
	}

	KeyBindings& SystemSettings::GetKeyBindings()
	{
		return SettingsHolder.Keys;
	}

	void SystemSettings::LoadAllSettingsFromConfigFile(const string& configName)
	{
		GetVideoSettings().LoadSettingsFromConfig(configName);
		GetAudioSettings().LoadSettingsFromConfig(configName);
	}

	VideoSettings::VideoSettings()
	{

	}

	void VideoSettings::LoadSettingsFromConfig(const string& configName)
	{
		RefPtr<Configuration> config = ConfigurationManager::Instance()->GetConfiguration(configName);
		ConfigName = configName;

		this->RenderSystem = config->FindOptionValue("RenderSystem", "Video.RenderSystem");
		this->IsFullscreen = config->FindOptionValue("Full Screen", "Video.Settings").compare("Yes") == 0 ? true : false;
		this->VideoMode = config->FindOptionValue("Video Mode", "Video.Settings");
		this->IsVSync = config->FindOptionValue("VSync", "Video.Settings").compare("Yes") == 0 ? true : false;
		this->FSAA = std::stoi(config->FindOptionValue("FSAA", "Video.Settings"));
	}

	void VideoSettings::SaveSettingsToConfig()
	{
		SaveSettingsToConfig(ConfigName);
	}

	void VideoSettings::SaveSettingsToConfig(const string& configName)
	{
		RefPtr<Configuration> config = ConfigurationManager::Instance()->GetConfiguration(ConfigName);
		Option* optRenderSystem = config->FindOption("RenderSystem", "Video.RenderSystem");
		optRenderSystem->Value = RenderSystem ;
		Option* optFullscreen = config->FindOption("Full Screen", "Video.Settings");
		optFullscreen->Value = IsFullscreen ? "Yes" : "No";
		Option* optVideoMode = config->FindOption("Video Mode", "Video.Settings");
		optVideoMode->Value = VideoMode;
		Option* optVSync = config->FindOption("VSync", "Video.Settings");
		optVSync->Value = std::to_string(IsVSync);
		Option* optFSAA = config->FindOption("FSAA", "Video.Settings");
		optFSAA->Value = std::to_string(FSAA);
		config->Save();
	}

	AudioSettings::AudioSettings() :
		MasterVolume(0.5f),
		GUIVolume(1.f),
		MusicVolume(1.f),
		AlertsVolume(1.f),
		VoiceVolume(1.f),
		ResponseVolume(1.f),
		GameVolme(1.f)
	{ }

	void AudioSettings::LoadSettingsFromConfig(const string& configName)
	{
		RefPtr<Configuration> config = ConfigurationManager::Instance()->GetConfiguration(configName);
		ConfigName = configName;
	}

	void AudioSettings::SaveSettingsToConfig()
	{
		SaveSettingsToConfig(ConfigName);
	}

	void AudioSettings::SaveSettingsToConfig(const string& configName)
	{
		RefPtr<Configuration> config = ConfigurationManager::Instance()->GetConfiguration(ConfigName);
	}
}