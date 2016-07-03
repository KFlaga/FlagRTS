#pragma once

#include "Sound.h"

namespace FlagRTS
{
	namespace Media
	{
		class OggSound : public Sound
		{
		public:
			OggSound();
			OggSound(const char* name, const char* pack, const char* file);
			~OggSound();

			void LoadSound();
			void UnloadSound();

		protected:
			string ALErrorToString(int error);	
		};
	}
}