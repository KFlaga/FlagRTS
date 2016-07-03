#include "WavSound.h"
#include "Array.h"
#include "Exception.h"

#define BUFFER_SIZE 4096*8

namespace FlagRTS
{
	using namespace Media;

	WavSound::WavSound() :
		Sound()
	{
		_extension = Wav;
	}

	WavSound::WavSound(const char* name, const char* pack, const char* file) :
		Sound(name, pack, file)
	{
		_extension = Wav;
	}

	WavSound::~WavSound()
	{
		if( _state != Unloaded )
			UnloadSound();
	}

	void WavSound::LoadSound()
	{
		unsigned char* soundBuffer;
		FILE* file;

		if(!(file = fopen(_filename.c_str(), "rb")))
		{
			CastException_d((string("Could not open Wav file: ") + _filename).c_str());
			return;
		}

		// Read file contents
		char xbuffer[5];
		xbuffer[4] = '\0';
		char buffer32[5];
		buffer32[4] = '\0';
		char buffer16[3];
		buffer16[2] = '\0';
		// 1) Header checks
		if (fread(xbuffer, sizeof(char), 4, file) != 4 || strcmp(xbuffer, "RIFF") != 0)
		{
			CastException_d((string("Bad format Wav file: ") + _filename).c_str());
			return;
		}

		fread(xbuffer, sizeof(char), 4, file); // Skip 4 bytes ( size )

		if (fread(xbuffer, sizeof(char), 4, file) != 4 || strcmp(xbuffer, "WAVE") != 0)
		{
			CastException_d((string("Bad format Wav file: ") + _filename).c_str());
			return;
		}

		if (fread(xbuffer, sizeof(char), 4, file) != 4 || strcmp(xbuffer, "fmt ") != 0)
		{
			CastException_d((string("Bad format Wav file: ") + _filename).c_str());
			return;
		}

		fread(xbuffer, sizeof(char), 4, file); // Skip 4 bytes
		// Read format info
		fread(buffer16, sizeof(char), 2, file);
		unsigned short audioFormat;
		sscanf(buffer16, "%i", &audioFormat);
		// Read channels num
		fread(buffer16, sizeof(char), 2, file);
		unsigned short channels;
		sscanf(buffer16, "%i", &channels);
		// Read sample rate
		fread(buffer32, sizeof(char), 4, file);
		unsigned int sampleRate;
		sscanf(buffer32, "%i", &sampleRate);
		// Read byterate
		fread(buffer32, sizeof(char), 4, file);
		unsigned int byteRate;
		sscanf(buffer32, "%i", &byteRate);
		// Skip 2 bytes
		fread(xbuffer, sizeof(char), 2, file); 
		// Read bits per sample
		fread(buffer16, sizeof(char), 2, file);
		unsigned short bitsPerSample;
		sscanf(buffer16, "%i", &bitsPerSample);

		// Read next 4 bytes
		if (fread(xbuffer, sizeof(char), 4, file) != 4 )
		{
			CastException_d((string("Bad format Wav file: ") + _filename).c_str());
			return;
		}

		if(strcmp(xbuffer,"fact") == 0) // Optional arguments to omit
		{
			// SKip 8 bytes and read next 4
			fread(xbuffer, sizeof(char), 4, file);
			fread(xbuffer, sizeof(char), 4, file);
			fread(xbuffer, sizeof(char), 4, file);
		}

		if (strcmp(xbuffer, "data") != 0)
		{
			CastException_d((string("Bad format Wav file: ") + _filename).c_str());
			return;
		}

		if(channels == 1)
			_alFormat = AL_FORMAT_MONO16;
		else
			_alFormat = AL_FORMAT_STEREO16;

		// Read remainig size of actual data
		fread(buffer32, sizeof(char), 4, file);
		int size;
		sscanf(buffer32, "%i", &size);

		// generate AL buffer
		alGenBuffers(1, &_alBufferId);
		CheckALError(__FUNCTION__);

		// Alloc memory for sound buffer and read data
		soundBuffer = (unsigned char*)std::malloc(size);
		fread(soundBuffer, sizeof(char), size, file);

		alBufferData(_alBufferId, _alFormat, soundBuffer, size, sampleRate);
		
		free(soundBuffer);
		fclose(file);

		UpdateLength();

		_state = SoundState::Loaded;
	}

	void WavSound::UnloadSound()
	{
		// Reset and delete buffer
		alDeleteBuffers(1, &_alBufferId);
		CheckALError(__FUNCTION__);

		_state = SoundState::Unloaded;
	}
}