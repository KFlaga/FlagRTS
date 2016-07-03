#include "Sound.h"
#include "Exception.h"
#include "Memory.h"
#include <OgreVector3.h>
#include "StringUtils.h"

namespace FlagRTS
{
	using namespace Media;

	Sound::Sound() :
		_state(Unloaded)
	{ 

	}

	Sound::Sound(const string& name, const string& pack, const string& file) :
		_state(Unloaded),
		_alBufferId(0),
		_name(name),
		_pack(pack),
		_filename(file)
	{ 

	}

	Sound::~Sound()
	{

	}

	void Sound::CheckALError(const string& source)
	{
		int error = alGetError();

		if(error != AL_NO_ERROR)
		{
			CastException_d((string("OpenAL Error: ") + ALErrorToString(error) + "In function: " + source).c_str());
		}
	}

	string Sound::ALErrorToString(int error)
	{
		switch(error)
		{
		case AL_INVALID_VALUE:
			return string("The value pointer given is not valid");
			break;
		case AL_INVALID_ENUM:
			return string("The specified parameter is not valid");
			break;
		case AL_INVALID_NAME:
			return string("The specified source name is not valid");
			break;
		case AL_INVALID_OPERATION:
			return string("There is no current context");
			break;
		default:
			return string("Unknown Error");
			break;
		}
	}

	void Sound::UpdateLength()
	{
		ALint sizeInBytes;
		ALint channels;
		ALint bits;
		ALint frequency;

		alGetBufferi(_alBufferId, AL_SIZE, &sizeInBytes);
		alGetBufferi(_alBufferId, AL_CHANNELS, &channels);
		alGetBufferi(_alBufferId, AL_BITS, &bits);
		alGetBufferi(_alBufferId, AL_FREQUENCY, &frequency);

		_length = 1000.f * ((float)sizeInBytes * 8.f) / ((float)(channels * bits) * (float)frequency);
	}
}