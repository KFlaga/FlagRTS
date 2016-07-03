#pragma once

#include "ISoundPlayer.h"

namespace FlagRTS
{
	namespace Media
	{
		// Allows only for one source to be playing same sound
		// If sound have same/lower priority and new request arrives it will be reset
		// else it won't play
		class SingleSoundPlayer : public ISoundPlayer
		{
			SoundSource* _currentSource;

		public:
			SingleSoundPlayer();
			SingleSoundPlayer(const char* soundName, const char* soundPack);
			SingleSoundPlayer(Sound* sound);

			void Play(SoundSource* ssource);
			void Pause(SoundSource* ssource);
			void Stop(SoundSource* ssource);
		};

		// Never ends sound if one is already playing
		class SingleNoStopSoundPlayer : public ISoundPlayer
		{
			SoundSource* _currentSource;

		public:
			SingleNoStopSoundPlayer();
			SingleNoStopSoundPlayer(const char* soundName, const char* soundPack);
			SingleNoStopSoundPlayer(Sound* sound);

			void Play(SoundSource* ssource);
			void Pause(SoundSource* ssource);
			void Stop(SoundSource* ssource);
		};

		// Allows only one source ( same source is returned if new one is requested
		// (bg music should be played directly in AudioManager however ))
		class MusicSoundPlayer : public ISoundPlayer
		{
			SoundSource* _currentSource;

		public:
			MusicSoundPlayer();
			MusicSoundPlayer(const char* soundName, const char* soundPack);
			MusicSoundPlayer(Sound* sound);

			SoundSource* RequestNewSource();

			void Play(SoundSource* ssource);
			void Pause(SoundSource* ssource);
			void Stop(SoundSource* ssource);
		};

		// Allows multiple sounds to be played -> if more than max number of this sound
		// needs to played, then first one is repeated
		class RevolverSoundPlayer : public ISoundPlayer
		{
			Array<SoundSource*> _sources;
			int _nextSource;
			int _maxPlays;

		public:
			RevolverSoundPlayer(int maxPlays);
			RevolverSoundPlayer(const char* soundName, 
				const char* soundPack,
				int maxPlays);
			RevolverSoundPlayer(Sound* sound,
				int maxPlays);

			void Play(SoundSource* ssource);
			void Pause(SoundSource* ssource);
			void Stop(SoundSource* ssource);
		};
	}
}