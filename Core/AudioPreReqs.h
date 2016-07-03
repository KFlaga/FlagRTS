#pragma once

#include "TypeDefs.h"
#include <AL\al.h>
#include <AL\alc.h>
#include <AL\alut.h>

namespace FlagRTS
{
	namespace Media
	{
		typedef size_t SoundHandle;

		typedef ALenum BufferType;
		typedef ALenum State;
		typedef ALuint SourceId;
		typedef ALuint BufferId;
		typedef ALsizei SizeType;

		enum SoundState
		{
			Unloaded = 0,
			Loaded = AL_INITIAL,
			Stopped = AL_STOPPED,
			Playing = AL_PLAYING,
			Paused = AL_PAUSED,
		};

		enum SoundExtension
		{
			Ogg, 
			Wav
		};

		enum SoundFormatChannels
		{
			Mono,
			Stereo,
			Quad,
			Multi_51,
			Multi_61,
			Multi_71,
			SoundFormatsCount,
		};

		struct SoundFormat
		{
			ALenum AlFormat;
			SoundFormatChannels Channels;
			const char* Name;

			SoundFormat() { }
			SoundFormat(ALenum alfromat, SoundFormatChannels channels, const char* name) :
				AlFormat(alfromat),
				Channels(channels),
				Name(name)
			{ }
		};

		// Due to possible limit to Sources count, sounds will be prioritized
		// if there's no space for higher priority sound, lower priority will be
		// stopped in first place
		enum SoundPriority
		{
			Low = 0,
			Normal = 1,
			High = 2,
			Highest = 3
		};
	}
}