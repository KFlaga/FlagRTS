#pragma once

#include "Sound.h"

namespace FlagRTS
{
	namespace Media
	{
		class WavSound : public Sound
		{
		public:
			WavSound();
			WavSound(const char* name, const char* pack, const char* file);
			~WavSound();

			void LoadSound();
			void UnloadSound();
		};
	}
}