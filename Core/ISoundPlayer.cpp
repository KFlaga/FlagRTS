#pragma once

#include "ISoundPlayer.h"
#include "SoundSource.h"
#include "Sound.h"
#include "AudioManager.h"
#include "SystemSettings.h"
#include "AudioSettings.h"

namespace FlagRTS
{
	using namespace Media;

	ISoundPlayer::ISoundPlayer() :
		_sound(0)
	{ }

	ISoundPlayer::ISoundPlayer(const char* soundName, const char* soundPack)
	{
		SetSound(soundName, soundPack);
	}

	ISoundPlayer::ISoundPlayer(Sound* sound)
	{
		SetSound(sound);
	}

	ISoundPlayer::~ISoundPlayer()
	{
		// Release all sources
		for(unsigned int i = 0; i < _allSources.size(); ++i)
		{
			AudioManager::Instance()->DestroySoundSource( _allSources[i] );
		}
	}

	void ISoundPlayer::SetSound(const char* soundName, const char* soundPack)
	{
		_sound = AudioManager::Instance()->FindSound(soundName, soundPack);
		for(unsigned int i = 0; i < _allSources.size(); ++i)
		{
			_allSources[i]->SetSound(_sound);
		}
	}

	void ISoundPlayer::SetSound(Sound* sound)
	{
		_sound = sound;
		for(unsigned int i = 0; i < _allSources.size(); ++i)
		{
			_allSources[i]->SetSound(_sound);
		}
	}

	SoundSource* ISoundPlayer::RequestNewSource()
	{
		SoundSource* ssource;
		if(_freeSources.size() > 0)
		{
			ssource = _freeSources.back();
			_freeSources.pop_back();
		}
		else
			ssource = AudioManager::Instance()->CreateSoundSource(_sound);

		ssource->SetGain( SystemSettings::GetAudioSettings().GameVolme );
		return ssource;
	}

	void ISoundPlayer::ReleaseSource(SoundSource* ssource)
	{
		_freeSources.push_back(ssource);
	}
}