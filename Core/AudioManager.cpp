#include "AudioManager.h"
#include "SoundFactory.h"
#include "SoundListener.h"
#include "Sound.h"
#include "SoundSource.h"
#include "Exception.h"
#include "ConfigurationManager.h"
#include "ErrorManager.h"
#include "LogManager.h"
#include <OgreSceneNode.h>
#include "SoundChannel.h"
#include "StringUtils.h"
#include "SystemSettings.h"
#include "AudioSettings.h"

namespace FlagRTS
{
	using namespace Media;

	AudioManager* AudioManager::_instance = 0;

	AudioManager::AudioManager() : 
		_soundFactory(xNew0(SoundFactory)),
		_playerFactory(xNew0(SoundPlayerFactory)),
		_soundListener(xNew0(SoundListener)),
		_onSoundFinished(this),
		_onMusicFinished(this),
		_listenerNode(0)
	{

	}

	AudioManager::~AudioManager()
	{
		ADD_LOG("Releasing OgreAL");
		ClearMusicQueue();
		xDelete(_backgroundMusicSource);
		// Stop and release all alsources
		for(unsigned int i = 0; i < _allALSources.size(); ++i)
		{
			alSourceStop(_allALSources[i]);
			alSourcei(_allALSources[i], AL_BUFFER, 0);
			alDeleteSources(1, &_allALSources[i]);
		}

		UnloadAllSounds();
		// First players, then sounds should be deleted
		for(auto packIt = _soundPlayers.begin(); packIt != _soundPlayers.end(); ++packIt)
		{
			for(auto it = packIt->Value->begin(); it != packIt->Value->end(); ++it)
			{
				xDelete( it->Value );
			}
			xDelete( packIt->Value );
		}
		_soundPlayers.clear();

		_loadedSounds.clear();

		for(auto packIt = _availableSounds.begin(); packIt != _availableSounds.end(); ++packIt)
		{
			for(auto it = packIt->Value->begin(); it != packIt->Value->end(); ++it)
			{
				xDelete( it->Value );
			}
			xDelete( packIt->Value );
		}
		_availableSounds.clear();

		for(auto chIt = _channels.begin(); chIt != _channels.end(); ++chIt)
		{
			xDelete( *chIt );
		}
		_channels.clear();

		ALCcontext *context = alcGetCurrentContext();
		ALCdevice *device = alcGetContextsDevice(context);
		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);

		xDelete(_soundFactory);
		xDelete(_playerFactory);
		xDelete(_soundListener);
	}

	ErrorCode AudioManager::InitSoundManager()
	{
		ADD_LOG("Initializing AudioManager");
		ADD_LOG("Creating OpenAL");

		auto availableDevices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
		ALCdevice* device = alcOpenDevice(NULL); // Open default device -> later may make it changable in options and read from config

		ADD_LOG("Creating device: " + string(alcGetString(device, ALC_DEVICE_SPECIFIER)));

		if(device == NULL)
		{
			CastException((string("Failed to open audio device") + alcGetString(device, ALC_DEVICE_SPECIFIER)).c_str());
		}

		ALCcontext* context = alcCreateContext(device, NULL);
		if(context == NULL)
		{
			CastException("Failed to create OpenAL Context");
		}

		alcMakeContextCurrent(context);
		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

		// Check for Supported Formats
		ALenum alFormat = 0;
		alFormat = alcGetEnumValue(device, "AL_FORMAT_MONO16");
		if(alFormat)
			_availableFormats.insert(alFormat, SoundFormat(alFormat, SoundFormatChannels::Mono, "Mono Sound"));
		alFormat = alcGetEnumValue(device, "AL_FORMAT_STEREO16");
		if(alFormat) 
			_availableFormats.insert(alFormat, SoundFormat(alFormat, SoundFormatChannels::Stereo, "Stereo Sound"));
		alFormat = alcGetEnumValue(device, "AL_FORMAT_QUAD16");
		if(alFormat) 
			_availableFormats.insert(alFormat, SoundFormat(alFormat, SoundFormatChannels::Quad, "4-CH Sound"));
		alFormat = alcGetEnumValue(device, "AL_FORMAT_51CHN16");
		if(alFormat) 
			_availableFormats.insert(alFormat, SoundFormat(alFormat, SoundFormatChannels::Multi_51, "5.1 Surround Sound"));
		alFormat = alcGetEnumValue(device, "AL_FORMAT_61CHN16");
		if(alFormat) 
			_availableFormats.insert(alFormat, SoundFormat(alFormat, SoundFormatChannels::Multi_61, "6.1 Surround Sound"));
		alFormat = alcGetEnumValue(device, "AL_FORMAT_71CHN16");
		if(alFormat) 
			_availableFormats.insert(alFormat, SoundFormat(alFormat, SoundFormatChannels::Multi_71, "7.1 Surround Sound"));

		// Check for EAX Support
		/*std::stringbuf versionString;
		for(int version = 5; version >= 2; version--)
		{
		versionString.str("EAX" + std::to_string(version) + ".0");
		if(alIsExtensionPresent(versionString.str().data()) == AL_TRUE)
		{
		_eaxSupport = true;
		_eaxVersion = version;
		break;
		}
		}*/

		//// Check for X-RAM extension
		//if(alIsExtensionPresent("EAX-RAM") == AL_TRUE)
		//{
		//	mXRAMSupport = true;
		//	Ogre::LogManager::getSingleton().logMessage("X-RAM Detected");

		//	EAXSetBufferMode setXRamMode = (EAXSetBufferMode)alGetProcAddress("EAXSetBufferMode");
		//	EAXGetBufferMode getXRamMode = (EAXGetBufferMode)alGetProcAddress("EAXGetBufferMode");
		//	mXRamSize = alGetEnumValue("AL_EAX_RAM_SIZE");
		//	mXRamFree = alGetEnumValue("AL_EAX_RAM_FREE");
		//	
		//	Ogre::LogManager::getSingleton().logMessage("X-RAM: " + Ogre::StringConverter::toString(mXRamSize) +
		//		" (" + Ogre::StringConverter::toString(mXRamFree) + " free)");
		//}

		alGetError();

		int sourcesCount = 0;
		// Create as many sources as possible ( up to 31 )
		while( alGetError() == AL_NO_ERROR 
			&& sourcesCount < 31 )
		{
			SourceId source;
			alGenSources(1, &source);
			_allALSources.push_back(source);
			_freeALSources.push_back(source);
			++sourcesCount;
		}

		if(sourcesCount == 0) // Somthing went bad
		{
			CastException("Couldnt generate any OpenAL source!");
		}

		// Create source for music
		_backgroundMusicSource = xNew0(SoundSource);
		_backgroundMusicSource->SetALSource(_allALSources.back());
		_backgroundMusicSource->GetOnFinsihedEvent() += &_onMusicFinished;
		_freeALSources.pop_back();
		_currentMusic = 0;

		_soundListener->SetGain(0.6f);

		// Create standard channels ( just next CH_StdChannelsCount )
		for(int i = 0; i < SoundChannel::CH_StdChannelsCount; ++i)
		{
			CreateChannel();
		}
		UpdateSettings();

		return ErrorCodes::Success;
	}

	void AudioManager::AddSoundDoc(const string& dir)
	{
		RefPtr<XmlDocument> soundDoc = XmlUtility::XmlDocFromFile((dir+"Sounds.xml").c_str());
		// Get first sound pack and check if any pack exists
		XmlNode* rootNode = soundDoc->first_node("SoundPackList");
		XmlNode* spackNode = rootNode->first_node("SoundPack", 9);
		if(spackNode != 0)
		{
			bool lastPackParsed = false; // true if all sounds packs are parsed
			do
			{
				// Check if its last loop
				if(spackNode == rootNode->last_node("SoundPack", 9))
					lastPackParsed = true;

				// Check if pack exists in map ( pack can be split into many nodes ) and find it
				char* packName = spackNode->first_attribute("Name", 4, false)->value();
				char* nameCopy = CopyChar(packName);
				if(_availableSounds.find(packName) == _availableSounds.end() )
				{
					// No such pack in map, so add new one
					_availableSounds.insert(nameCopy, (xNew0(SoundPack)));
					_soundPlayers.insert(nameCopy, xNew0(HashedMaps<ISoundPlayer*>::KeyCCString));
				}
				SoundPack* soundPack = _availableSounds.find(packName)->Value;

				ParseSoundPackNode(spackNode, soundPack, dir);

				// Get next sound pack node
				spackNode = spackNode->next_sibling("SoundPack", 9);
			}
			while(!lastPackParsed);
		}
	}

	void AudioManager::ParseSoundPackNode(XmlNode* spackNode, SoundPack* soundPack, const string& packDir)
	{

		// Parse all sounds in pack
		XmlNode* soundNode = spackNode->first_node("Sound", 5);
		if(soundNode != 0)
		{
			bool lastSoundParsed = false;
			do
			{
				// Check if its last loop
				if(soundNode == spackNode->last_node("Sound", 5))
					lastSoundParsed = true;

				Sound* sound = _soundFactory->Create(soundNode);
				if(sound != 0)
				{
					// add directory to sound file path
					sound->SetFile( packDir + sound->GetFile());

					soundPack->insert(sound->GetName().c_str(), sound);
					ISoundPlayer* player = _playerFactory->Create(soundNode);
					if(player != 0)
					{
						player->SetSound(sound);
						_soundPlayers[sound->GetPack().c_str()]->insert(sound->GetName().c_str(), player);
					}
				}

				// Get next sound from pack
				soundNode = soundNode->next_sibling("Sound", 5);
			}
			while(!lastSoundParsed);
		}
	}

	ErrorCode AudioManager::LoadSoundPack(const char* packName)
	{
		if(_loadedSounds.find(packName) != _loadedSounds.end() )
			return RETURN_ERROR(ErrorCodes::AudioPackAlreadyLoaded);
		if(_availableSounds.find(packName) == _availableSounds.end() )
			return RETURN_ERROR(ErrorCodes::AudioPackNotFound);

		SoundPack* pack = _availableSounds.find(packName)->Value;
		_loadedSounds[packName] = pack;

		for(auto sit = pack->begin(); sit != pack->end(); sit++)
		{
			sit->Value->LoadSound();
		}
		return ErrorCodes::Success;
	}

	ErrorCode AudioManager::UnloadSoundPack(const char* packName)
	{
		if(_loadedSounds.find(packName) != _loadedSounds.end())
			return RETURN_ERROR(ErrorCodes::AudioPackNotLoaded);

		RefPtr<SoundPack> pack = _loadedSounds.find(packName)->Value;
		_loadedSounds.erase(packName);

		for(auto sit = pack->begin(); sit != pack->end(); sit++)
		{
			sit->Value->UnloadSound();
		}
		return ErrorCodes::Success;
	}

	ErrorCode AudioManager::UnloadAllSounds()
	{
		for(auto spit = _loadedSounds.begin(); spit != _loadedSounds.end(); spit++)
		{
			for(auto sit = spit->Value->begin(); sit != spit->Value->end(); sit++)
			{
				sit->Value->UnloadSound();
			}
		}
		_loadedSounds.clear();
		return ErrorCodes::Success;
	}

	Sound* AudioManager::FindSound(const char* soundName, const char* soundPack)
	{
		auto packIt = _availableSounds.find(soundPack);
		if(packIt != _availableSounds.end())
		{
			auto soundIt = packIt->Value->find(soundName);
			if(soundIt != packIt->Value->end())
			{
				return soundIt->Value;
			}
		}
		return 0; // Pack/sound not found
	}

	void AudioManager::Update(float ms)
	{
		// Update listner
		if( _listenerNode != 0)
		{
			auto& pos = _listenerNode->getPosition();
			if( !_soundListener->GetPosition().positionEquals(pos, 0.1f) )
			{
				_soundListener->SetVelocity( (pos - _soundListener->GetPosition() ) / ms );
				_soundListener->SetPosition(pos);
			}
			else
				_soundListener->SetVelocity(Vector3(0.f,0.f,0.f));
			auto& orient = _listenerNode->getOrientation();
			Vector3 newDir = orient * Vector3(0.f,0.f,-1.f);
			if( !newDir.directionEquals(_soundListener->GetDirection(), Radian(Degree(1.f))) )
			{
				_soundListener->SetDirection(newDir);
			}
			Vector3 newUp = orient * Vector3(0.f,1.f,0.f);
			if( !newUp.directionEquals(_soundListener->GetDirection(), Radian(Degree(1.f))) )
			{
				_soundListener->SetUp(newUp);
			}
		}
		_soundListener->Update(ms);

		// Pause/stop/play sounds from queues
		for(auto sit = _pauseQueue.begin(); sit != _pauseQueue.end(); ++sit)
		{
			PauseSoundInt( *sit );
		}
		_pauseQueue.clear();

		for(auto sit = _stopQueue.begin(); sit != _stopQueue.end(); ++sit)
		{
			StopSoundInt( *sit );
		}
		_stopQueue.clear();

		for(auto sit = _playQueue.begin(); sit != _playQueue.end(); ++sit)
		{
			StartSoundInt( *sit );
		}
		_playQueue.clear();

		// Update all sources
		for(auto sourceIt = _usedSources.begin(); sourceIt != _usedSources.end(); ++sourceIt)
		{
			sourceIt->Value->Update(ms);
		}
	}

	void AudioManager::AttachToScene(Ogre::SceneNode* snode)
	{
		_listenerNode = snode;
		_soundListener->SetPosition(snode->getPosition());
		auto& orient = snode->getOrientation();
		_soundListener->SetDirection(orient*Vector3(0.f,0.f,-1.f));
		_soundListener->SetUp(orient*Vector3(0.f,1.f,0.f));
		_soundListener->SetVelocity(Vector3(0.f,0.f,0.f));
	}

	SoundSource* AudioManager::CreateSoundSource(Sound* sound)
	{
		SoundSource* ssource = xNew0(SoundSource);
		if(sound->GetCurrentState() == Unloaded)
		{
			sound->LoadSound();
		}
		ssource->SetSound( sound );
		return ssource;
	}

	SoundSource* AudioManager::CreateSoundSource(const char* soundName, const char* soundPack)
	{
		SoundSource* ssource = xNew0(SoundSource);
		Sound* sound = FindSound(soundName, soundPack);
		if(sound->GetCurrentState() == Unloaded)
		{
			sound->LoadSound();
		}
		ssource->SetSound(sound);
		return ssource;
	}

	void AudioManager::DestroySoundSource(SoundSource* ssource)
	{
		if(ssource->GetCurrentState() != Unloaded)
		{
			StopSoundInt(ssource);
			ssource->SetSound(0);
		}
		xDelete(ssource);
	}

	ISoundPlayer* AudioManager::GetSoundPlayer(const char* soundName, const char* soundPack)
	{
		auto packIt = _soundPlayers.find(soundPack);
		if(packIt != _soundPlayers.end())
		{
			auto soundIt = packIt->Value->find(soundName);
			if(soundIt != packIt->Value->end())
			{
				return soundIt->Value;
			}
		}
		return 0; // Pack/sound not found
	}

	void AudioManager::PlaySoundSource(SoundSource* soundSource)
	{
		_playQueue.push_back(soundSource);
	}

	void AudioManager::StopSoundSource(SoundSource* soundSource)
	{
		_stopQueue.push_back(soundSource);
	}

	void AudioManager::PauseSoundSource(SoundSource* soundSource)
	{
		_pauseQueue.push_back(soundSource);
	}

	void AudioManager::StartSoundInt(SoundSource* ssource)
	{
		switch( ssource->GetCurrentState() )
		{
		case Playing:
			// If already playing, play again
			ssource->Rewind();
			break;
		case Paused:
			// If was paused, resume
			ssource->Start();
			break;
		case Stopped:
			// If was stopped, but not unloaded ( so both source and sound are set )
			ssource->Start();
			break;
		default: // Unloaded -> source should be set to 0, but not sound
			_ASSERT( ssource->GetSound() != 0 );
			if( ssource->GetSound() == 0 )
				return;
			_ASSERT( ssource->GetSound()->GetAlBuffer() != 0 );
			if( ssource->GetSound()->GetAlBuffer() == 0 )
				return;

			// Check if there is free source
			if( _freeALSources.size() == 0 )
			{
				// If last sound have lower priority  ( it have lowest in list ) - end it
				if( _usedSources.back()->GetPriority() <= ssource->GetPriority() )
				{
					auto toRemove = _usedSources.back();
					_usedSources.pop_back();
					_freeALSources.push_back(toRemove->GetAlSource());
					toRemove->Stop();
					toRemove->SetALSource(0);
				}
			}
			ssource->SetALSource( _freeALSources.back() );
			_freeALSources.pop_back();
			ssource->Start();
			// Push it to correct place at used list
			auto sourceIt = _usedSources.begin();
			for(; sourceIt != _usedSources.end(); ++sourceIt)
			{
				// Find first source with lower/same priority and place it before it
				if( sourceIt->Value->GetPriority() <= ssource->GetPriority() )
				{
					if( sourceIt == _usedSources.begin() ) // New sound have highest prioirty in list -> push front
						_usedSources.push_front( ssource );
					else
						_usedSources.insert( sourceIt, ssource );
					break;
				}
			}
			if( sourceIt == _usedSources.end() )
			{
				// list empty or only higher priorities -> push_back
				_usedSources.push_back( ssource );
			}
			ssource->GetOnFinsihedEvent() += &_onSoundFinished;
		}
	}

	void AudioManager::PauseSoundInt(SoundSource* ssource)
	{
		if( ssource->GetCurrentState() == Playing )
		{
			ssource->Pause();
		}
	}

	void AudioManager::StopSoundInt(SoundSource* ssource)
	{
		ssource->GetOnFinsihedEvent() -= &_onSoundFinished;
		if( ssource->GetAlSource() != 0 )
		{
			_usedSources.remove( ssource );
			_freeALSources.push_back(ssource->GetAlSource());
			ssource->Rewind();
			ssource->Stop();
			ssource->SetALSource(0);
		}
	}

	void AudioManager::OnSoundFinished(SoundSource* ssource, bool finished)
	{
		if( finished )
		{
			StopSoundSource(ssource);
		}
	}

	void AudioManager::OnMusicFinished(SoundSource* ssource, bool finished)
	{
		if( finished )
		{
			NextMusic();
		}
	}

	void AudioManager::AddMusicToQueue(const char* soundName, const char* soundPack)
	{
		_musicQueue.push_back(FindSound(soundName, soundPack));
		if( _musicQueue.size() == 1)
			NextMusic();
	}

	void AudioManager::RemoveMusicFromQueue(const char* soundName, const char* soundPack)
	{
		Sound* sound =  FindSound(soundName, soundPack);

		if( sound == 0 )
			return;

		if( _backgroundMusicSource->GetSound() == sound)
		{
			if( _musicQueue.size() == 1 )
			{
				ClearMusicQueue();
				return;
			}
			else
				NextMusic();
		}

		for(unsigned int i = 1; i < _musicQueue.size(); ++i)
		{
			if( _musicQueue[i] == sound )
				_musicQueue.removeAt(i);
		}
	}

	void AudioManager::ClearMusicQueue()
	{
		_musicQueue.clear();
		if( _backgroundMusicSource->GetCurrentState() != Unloaded )
		{
			_backgroundMusicSource->Stop();
		}
		_backgroundMusicSource->SetSound(0);
		_backgroundMusicSource->SetALSource(0);
		_currentMusic = 0;
	}

	void AudioManager::NextMusic()
	{
		if( _musicQueue.size() == 0 )
			return;

		++_currentMusic;
		if( _currentMusic >= _musicQueue.size() )
			_currentMusic = 0;

		if( _backgroundMusicSource->GetCurrentState() == Playing )
			_backgroundMusicSource->Stop();
		_backgroundMusicSource->SetSound(_musicQueue[_currentMusic]);

		PlaySoundSource(_backgroundMusicSource);
	}

	void AudioManager::SetMusicGain(float mgain)
	{
		_backgroundMusicSource->SetGain(mgain);
	}

	int AudioManager::CreateChannel()
	{
		SoundChannel* channel = xNew1(SoundChannel, _channels.size());
		_channels.push_back( channel );
		return channel->GetId();
	}

	SoundChannel* AudioManager::GetChannel(int id)
	{
		_ASSERT( id < (int)_channels.size() );
		return _channels[id];
	}

	void AudioManager::UpdateSettings()
	{
		_soundListener->SetGain(SystemSettings::GetAudioSettings().MasterVolume);
		_backgroundMusicSource->SetGain(SystemSettings::GetAudioSettings().MusicVolume);

		SoundChannel* channel;
		for(unsigned int i = 0; i <_channels.size(); ++i)
		{
			channel = _channels[i];
			switch (channel->GetId())
			{
			case SoundChannel::CH_Gui:
				channel->SetGain(SystemSettings::GetAudioSettings().GUIVolume);
				break;
			case SoundChannel::CH_Alerts:
				channel->SetGain(SystemSettings::GetAudioSettings().AlertsVolume);
				break;
			case SoundChannel::CH_Dialog1:
			case SoundChannel::CH_Dialog2:
				channel->SetGain(SystemSettings::GetAudioSettings().VoiceVolume);
				break;
			case SoundChannel::CH_SelectionAcknowledge1:
			case SoundChannel::CH_SelectionAcknowledge2:
				channel->SetGain(SystemSettings::GetAudioSettings().ResponseVolume);
				break;

			default:
				break;
			}
		}
	}
}