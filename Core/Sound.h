#pragma once

#include "AudioPreReqs.h"
#include "Pointers.h"
#include "IGameObject.h"
#include <OgreVector3.h>

namespace FlagRTS
{
	namespace Media
	{
		// Basic sound : countains info about
		// sound name, file and soundpack and OpenAL info
		// It does not play sound itself - to do so its buffer need to be bind to some ALSource
		class Sound : public IGameObject
		{
		protected:
			string _name; // Name of sound
			string _pack; // Name of SoundPack sound belongs to
			string _filename; // Name of file with actual sound
			SoundState _state;
			SoundExtension _extension;
			ALenum _alFormat;
			float _length;
			BufferId _alBufferId; // Handle to ALBuffer bind to this sound

		public:
			// Creates uninitialized sound
			Sound();
			// Creates uninitialized and sets sound info
			Sound(const string& name, const string& pack, const string& file);

			virtual ~Sound();

			void SetName(const string& name) { _name = name; }
			const string& GetName() const { return _name; }
			void SetPack(const string& pack) { _pack = pack; }
			const string& GetPack() const { return _pack; }
			void SetFile(const string& file) { _filename = file; }
			const string& GetFile() const { return _filename; }

			SoundState GetCurrentState() const { return _state; }

			// Returns name in format 'pack'_'name'
			string GetFullName() const { return _pack + "_" + _name; }

			SoundExtension GetSoundExtension() const { return _extension; }

			BufferId GetAlBuffer() const { return _alBufferId; }

			ALenum GetAlFormat() const { return _alFormat; }

			// Creates OpenAL sound so it can be used later and returns it
			virtual void LoadSound() = 0;

			// Destroys sound - no longer can be used
			virtual void UnloadSound() = 0;

			// Calculates new length of sonud in buffer
			void UpdateLength();
			// Returns length in ms of sound
			float GetLength() const { return _length; }

		protected:
			void CheckALError(const string& source);
			string ALErrorToString(int error);	
		};
	}
}
