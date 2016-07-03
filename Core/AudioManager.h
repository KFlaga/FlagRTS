#pragma once

#include "GenericManager.h"
#include "Pointers.h"
#include "ArrayMap.h"
#include "List.h"
#include "Xml.h"
#include "Sound.h"
#include "ISoundPlayer.h"
#include "Event.h"
#include "HashMap.h"

namespace Ogre
{
	class SceneManager;
	class SceneNode;
}

namespace FlagRTS
{
	namespace Media
	{
		class SoundListener;
		class SoundFactory;
		class SoundPlayerFactory;
		class SoundSource;
		class ISoundPlayer;
		class SoundChannel;
		typedef HashedMaps<Sound*>::KeyCCString SoundPack;
		typedef ArrayMaps<SoundPack*>::KeyCCString SoundPackMap;
		typedef Array<SoundSource*> SoundQueue;
		typedef ArrayMaps<HashedMaps<ISoundPlayer*>::KeyCCString*>::KeyCCString SoundPlayerMap;

		// Allows play/pause/resume of sounds in ogg format
		// Internally uses OgreAL SoundManager, which allows for 3d sound -
		// having sound listener and sound source placed in 3d scene
		class AudioManager : public GenericManager<AudioManager>
		{
		private:
			SoundFactory* _soundFactory;
			SoundPlayerFactory* _playerFactory;
			SoundListener* _soundListener;
			Ogre::SceneNode* _listenerNode;

			SoundPackMap _availableSounds; // All sounds read from sound config
			SoundPackMap _loadedSounds; // Sounds created and ready to use
			SoundPlayerMap _soundPlayers;

			SoundQueue _playQueue; // Sounds to be played on next update
			SoundQueue _stopQueue; // Sounds to be stopped on next update
			SoundQueue _pauseQueue; // Sounds to be paused on next update

			Array<Sound*> _musicQueue; // Music sounds, which starts ine after another ( in loop ). Music changes on updates
			uint32 _currentMusic; // Position in queue of currently playing music

			ArrayMap<ALenum, SoundFormat> _availableFormats;
			bool _eaxSupport;
			int _eaxVersion;

			float _dopplerFactor;
			float _speedOfSound;
			
			// All generated AL sources
			Array<SourceId> _allALSources;
			// AL sources currently free
			Array<SourceId> _freeALSources;

			// Currently playing / paused SoundSources sorted by priority ( first highest )
			List<SoundSource*> _usedSources;
			SoundSource* _backgroundMusicSource;

			Array<SoundChannel*> _channels;

		protected:
			AudioManager();
			friend GenericManager<AudioManager>;

		public:
			~AudioManager();
			// Initializes sound engine and loads available sounds from sound config file
			ErrorCode InitSoundManager();
			// Parses Sounds.xml document from given dir and initializes sounds from it ( doesnt load them yet )
			void AddSoundDoc(const string& dir);

			// Loads soundpack into memory -> given file is searched for in sound config
			// file ( and this one from mainConfig ). In this file all sounds names 
			// to load should be placed. 
			ErrorCode LoadSoundPack(const char* packName);

			// Unloads all sounds from soundpack
			ErrorCode UnloadSoundPack(const char* packName);

			// Deletes all sounds
			ErrorCode UnloadAllSounds();

			// Attached sound listener to SceneNode ( actually it don't attach any object
			// but just copies node position )
			void AttachToScene(Ogre::SceneNode* snode);

			// Returns listener scene node, so it may be transformed
			Ogre::SceneNode* GetListenerNode() const { return _listenerNode; }
			// Set listener's node -> listener will move/rotate with node
			void SetListenerNode(Ogre::SceneNode* node) { _listenerNode = node; }

			// Returns Sound object with given name or 0 if not found
			Sound* FindSound(const char* soundName, const char* soundPack);

			Sound* FindSound(const string& soundName, const string& soundPack)
			{
				return FindSound(soundName.c_str(), soundPack.c_str());
			}

			// Creates SoundSource that plays given sound (loads sound if its unloaded)
			SoundSource* CreateSoundSource(Sound* sound);
			// Creates SoundSource that plays sound given by name (loads sound if its unloaded)
			SoundSource* CreateSoundSource(const char* soundName, const char* soundPack);
			SoundSource* CreateSoundSource(const string& soundName, const string& soundPack)
			{
				return CreateSoundSource(soundName.c_str(), soundPack.c_str());
			}

			// Safely destorys SoundSource
			void DestroySoundSource(SoundSource* ssource);

			// Returns sound player
			ISoundPlayer* GetSoundPlayer(const char* soundName, const char* soundPack);
			ISoundPlayer* GetSoundPlayer(const string& soundName, const string& soundPack)
			{
				return GetSoundPlayer(soundName.c_str(), soundPack.c_str());
			}

			// Queue playing of given sound source.
			// If all sources are used sound first sound with lowest priority will be stopped
			// Sounds qith Highest priorites are never stopped explicitly
			void PlaySoundSource(SoundSource* soundSource);
			// Queue stopping of given sound source
			void StopSoundSource(SoundSource* soundSource);
			// Queue pause of given sound source
			void PauseSoundSource(SoundSource* soundSource);

			// Returns sound listener -> that is object from which sound is 'heard'
			// BgMusic and gui sound etc should be played in listener position, so it is heard naturally
			// but sounds from evironment are heard as more or less distant and from
			// specified direction.
			// It can be attached to scene node ( camera, player etc )
			SoundListener* GetSoundListener() { return _soundListener; }

			// Handles pending sounds requests
			void Update(float ms);

			// Adds music to the end of music queue
			void AddMusicToQueue(const char* soundName, const char* soundPack);
			// Removes music from music queue ( stops it if playing )
			void RemoveMusicFromQueue(const char* soundName, const char* soundPack);
			// Removes all music from queue ( and stops currently playing )
			void ClearMusicQueue();
			// Changes music to next one
			void NextMusic();
			// Sets new music volume ( in range [0-1] )
			void SetMusicGain(float mgain);

			// Creates new sound channel and returns id to it
			int CreateChannel();
			// Returns channel with given id 
			SoundChannel* GetChannel(int id);
			// Updates all sounds gains after settings changes
			void UpdateSettings();

		private:
			// Parses XmlNode with sounds in pack and saves them in SoundPack
			void ParseSoundPackNode(XmlNode* spackNode, SoundPack* soundPack, const string& packDir);

			void StartSoundInt(SoundSource* ssource);
			void PauseSoundInt(SoundSource* ssource);
			void StopSoundInt(SoundSource* ssource);

			void OnSoundFinished(SoundSource* ssource, bool finished);
			DEFINE_DELEGATE2(SoundFinishedDelegate, AudioManager,
				OnSoundFinished, SoundSource*, bool);
			SoundFinishedDelegate _onSoundFinished;

			void OnMusicFinished(SoundSource* ssource, bool finished);
			DEFINE_DELEGATE2(MusicFinishedDelegate, AudioManager,
				OnMusicFinished, SoundSource*, bool);
			MusicFinishedDelegate _onMusicFinished;
		};
	}
}