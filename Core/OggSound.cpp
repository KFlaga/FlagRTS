#include "OggSound.h"
#include "Array.h"
#include "Exception.h"

#include <Ogg\ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define BUFFER_SIZE 4096*8

namespace FlagRTS
{
	using namespace Media;

	OggSound::OggSound() :
		Sound()
	{
		_extension = Ogg;
	}

	OggSound::OggSound(const char* name, const char* pack, const char* file) :
		Sound(name, pack, file)
	{
		_extension = Ogg;
	}

	OggSound::~OggSound()
	{
		if( _state != Unloaded )
			UnloadSound();
	}

	void OggSound::LoadSound()
	{
		int currSection;
		long size = 0;
		char data[BUFFER_SIZE];
		Array<char> buffer;

		FILE* oggFile;
		OggVorbis_File oggStream;
		vorbis_info* vorbisInfo;

		if(!(oggFile = fopen(_filename.c_str(), "rb")))
		{
			CastException_d((string("Could not open Ogg file: ") + _filename).c_str());
			return;
		}

		if(ov_open(oggFile, &oggStream, NULL, 0) < 0)
		{
			fclose(oggFile);
			CastException_d((string("Could not open Ogg stream: ") + _filename).c_str());
			return;
		}

		// Read file info
		vorbisInfo = ov_info(&oggStream, -1);
		SizeType freq = vorbisInfo->rate;

		if(vorbisInfo->channels == 1)
			_alFormat = AL_FORMAT_MONO16;
		else
			_alFormat = AL_FORMAT_STEREO16;

		// generate AL buffer
		alGenBuffers(1, &_alBufferId);
		CheckALError(__FUNCTION__);

		// Copy ogg stream to buffer
		do
		{
			size = ov_read(&oggStream, data, sizeof(data), 0, 2, 1, &currSection);
			buffer.insert(buffer.end(), data, data + size);
		}
		while(size > 0); // ov_read returns remaining size

		// Clear copied stream
		ov_clear(&oggStream);

		// Copy our buffer to AL
		alBufferData(_alBufferId, _alFormat, &buffer[0], static_cast<SizeType>(buffer.size()), freq);

		UpdateLength();

		_state = SoundState::Loaded;
	}

	void OggSound::UnloadSound()
	{
		// Reset and delete buffer
		alDeleteBuffers(1, &_alBufferId);
		CheckALError(__FUNCTION__);

		_state = SoundState::Unloaded;
	}

	string OggSound::ALErrorToString(int error)
	{
		switch(error)
		{
			case OV_EREAD:
				return string("Read from media.");
			case OV_ENOTVORBIS:
				return string("Not Vorbis data.");
			case OV_EVERSION:
				return string("Vorbis version mismatch.");
			case OV_EBADHEADER:
				return string("Invalid Vorbis header.");
			case OV_EFAULT:
				return string("Internal logic fault (bug or heap/stack corruption.");
			default:
				return string("Unknown Ogg error.");
		}
	}
}