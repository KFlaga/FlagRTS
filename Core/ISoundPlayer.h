#pragma once

#include "TypeDefs.h"
#include "IGameObject.h"
#include "Array.h"

namespace FlagRTS
{
	namespace Media
	{
		class Sound;
		class SoundSource;
		class AudioManager;

		// Class intended to be used for direct sound playing in app
		// Calls AudioManager methods to create SoundSource and Play/Stop
		// Typicaly one Sound and one SoundSource is set for one SoundPlayer and
		// all objects that want to play this sound should use same player, so'
		// sound is repeated when tried to play again
		// In multi sound implementation for one Sound can exists few sources and
		// limited number of them may be playing simultaneously
		// All SoundPlayers uses the same additional gain, that is GameVolume from AudioSettings
		// which is set when requesting SoundSource -> all SoundPlayer users should
		// multiply gain with one set, not set a new one
		class ISoundPlayer : public IGameObject
		{
		protected:
			Array<SoundSource*> _allSources;
			Array<SoundSource*> _freeSources;
			Sound* _sound;

		public:
			// Creates empty SoundPlayer
			ISoundPlayer();
			// Creates SoundPlayer with given sound
			ISoundPlayer(const char* soundName, const char* soundPack);
			// Creates SoundPlayer with given sound
			ISoundPlayer(Sound* sound);
			~ISoundPlayer();
			// Sets sound for player ( should be called only once )
			void SetSound(const char* soundName, const char* soundPack);
			// Sets sound for player ( should be called only once )
			void SetSound(Sound* sound);

			// Sound source for objects that want to play this player's sound
			virtual SoundSource* RequestNewSource();
			// Destory source when no longer needed
			void ReleaseSource(SoundSource* ssource);

			virtual void Play(SoundSource* ssource) = 0;
			virtual void Pause(SoundSource* ssource) = 0;
			virtual void Stop(SoundSource* ssource) = 0;
		};
	}
}