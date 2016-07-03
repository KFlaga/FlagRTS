#pragma once

#include "Sound.h"

namespace ORTS
{
	namespace Media
	{
		// Used when one sound is needed to play multiple times in same time
		// (like gunfires). Such sounds are played in circle ( every time next
		// ogre sound is played ).
		class MultiSound : public Sound
		{
		private:
			uint8 _maxPlays; // Max num of simul. sounds played
			string* _ogreSounds; // Arrays of names of ogre sounds to play
			uint8 _currentSound; // Currently played sounds

		public:
			MultiSound() : Sound(), _ogreSounds(0) {}

			MultiSound(uint8 maxPlays, const string& name, const string& pack, const string& file) :
				Sound(name, pack, file),
				_maxPlays(maxPlays),
				_ogreSounds(new string[maxPlays])
			{ }

			~MultiSound()
			{
				if(_ogreSounds != 0)
				{
					delete[] _ogreSounds;
					_ogreSounds = 0;
				}
			}

			void SetMaxPlays(uint8 mp)
			{
				_maxPlays = mp;
				if(_ogreSounds != 0)
					delete[] _ogreSounds;
				_ogreSounds = new string[mp];
			}

			// Loads all sounds with same source file
			void LoadSound();

			// Unloads all sounds
			void UnloadSound();

			// Increment sound counter
			void NextSound()
			{
				_currentSound++;
				if(_currentSound >= _maxPlays)
				{
					_currentSound = 0;
				}
			}
		};
	}
}