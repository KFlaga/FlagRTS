#include "SoundSource.h"
#include "Exception.h"
#include "Memory.h"
#include "Sound.h"
#include "AudioManager.h"

namespace FlagRTS
{
	using namespace Media;

	SoundSource::SoundSource() :
		_state(Unloaded),
		_playedSound(0),
		_alSourceId(0),
		_progress(0.f),
		_position(0.f,0.f,0.f),
		_direction(0.f,0.f,0.f),
		_velocity(0.f,0.f,0.f),
		_gain(1.f),
		_pitch(1.f),
		_maxAttDistance(1000.f),
		_halfDistance(100.f),
		_rolloff(1.f),
		_outerConeGain(0.f),
		_outerConeAngle(360.f),
		_innerConeAngle(360.f),
		_priority(Normal),
		_isLoop(false),
		_relativeToListener(true),
		_changed(false)
	{ 

	}

	SoundSource::~SoundSource()
	{
		// ALSource should be unhooked before destroying
		_ASSERT(_alSourceId == 0);
	}

	void SoundSource::SetALSource(SourceId alsource)
	{
		if( _playedSound != 0)
		{
			if(_alSourceId != 0)
				alSourcei(_alSourceId, AL_BUFFER, 0);
			if(alsource != 0)
			{
				alSourcei(alsource, AL_BUFFER, _playedSound->GetAlBuffer());
				_state = Stopped;
			}
			else
				_state = Unloaded;
		}
		_alSourceId = alsource;
	}

	void SoundSource::SetSound(Sound* sound)
	{
		if(_alSourceId != 0)
		{
			if(_playedSound != 0)
			{
				Stop();
				alSourcei(_alSourceId, AL_BUFFER, 0);
			}
			if(sound != 0)
			{
				_state = Stopped;
				if(_alSourceId != 0)
					alSourcei(_alSourceId, AL_BUFFER, sound->GetAlBuffer());
			}
			else
				_state = Unloaded;
		}

		_playedSound = sound;
		_progress = 0.f;
	}

	void SoundSource::Update(float ms)
	{
		if( _state == Playing )
		{
			_progress += ms / _playedSound->GetLength();
			if(_progress >= 1.f)
			{
				_onFinished.Fire(this, true);
				_progress = 0.f;
			}
			if(_changed == true && _relativeToListener == false)
			{
				alSource3f(_alSourceId, AL_POSITION, _position.x, _position.y, _position.z);
				alSource3f(_alSourceId, AL_VELOCITY, _velocity.x, _velocity.y, _velocity.z);
				alSource3f(_alSourceId, AL_DIRECTION, _direction.x, _direction.y, _direction.z);
				_changed = false;
			}
		}
	}

	void SoundSource::Start()
	{
		_ASSERT(_alSourceId != 0);
		_ASSERT(_playedSound != 0);
		SetSourceValues();
		_changed = false;
		_progress = 0.f;
		alSourcePlay(_alSourceId);
		_state = Playing;
	}

	void SoundSource::Pause()
	{
		_ASSERT(_alSourceId != 0);
		_ASSERT(_playedSound != 0);
		alSourcePause(_alSourceId);
		_state = Paused;
	}

	void SoundSource::Stop()
	{
		_ASSERT(_alSourceId != 0);
		_ASSERT(_playedSound != 0);
		alSourceStop(_alSourceId);
		_state = Stopped;
		_onFinished.Fire(this, false);
	}

	void SoundSource::Rewind()
	{
		_ASSERT(_alSourceId != 0);
		_ASSERT(_playedSound != 0);
		_progress = 0.f;
		alSourceRewind(_alSourceId);
	}

	void SoundSource::SetPitch(float pitch)
	{
		_pitch = pitch;
		if( _state == Playing )
			alSourcef(_alSourceId, AL_PITCH, pitch);
	}

	void SoundSource::SetGain(float gain)
	{
		_gain = gain;
		if( _state == Playing )
			alSourcef(_alSourceId, AL_GAIN, gain);
	}

	void SoundSource::SetMaxAttDistance(float maxDistance)
	{
		_maxAttDistance = maxDistance;
		if( _state == Playing )
			alSourcef(_alSourceId, AL_MAX_DISTANCE, maxDistance);
	}

	void SoundSource::SetRolloffFactor(float rolloffFactor)
	{
		_rolloff = rolloffFactor;
		if( _state == Playing )
			alSourcef(_alSourceId, AL_ROLLOFF_FACTOR, rolloffFactor);
	}

	void SoundSource::SetHalfVolumeDistance(float dist)
	{
		_halfDistance = dist;
		if( _state == Playing )
			alSourcef(_alSourceId, AL_REFERENCE_DISTANCE, dist);
	}

	void SoundSource::SetVelocity(const Vector3& vel)
	{
		_velocity = vel;
		_changed = true;
	}

	void SoundSource::SetPosition(const Vector3& vel)
	{
		_position = vel;
		_changed = true;
	}

	void SoundSource::SetDirection(const Vector3& vel)
	{
		_direction = vel;
		_changed = true;
	}

	void SoundSource::SetOuterConeGain(float outerConeGain)
	{
		_outerConeGain = outerConeGain;
		if( _state == Playing )
			alSourcef(_alSourceId, AL_CONE_OUTER_GAIN, outerConeGain);
	}

	void SoundSource::SetInnerConeAngle(float innerConeAngle)
	{
		_innerConeAngle = innerConeAngle;
		if( _state == Playing )
			alSourcef(_alSourceId, AL_CONE_INNER_ANGLE, innerConeAngle);
	}

	void SoundSource::SetOuterConeAngle(float outerConeAngle)
	{
		_outerConeAngle = outerConeAngle;
		if( _state == Playing )
			alSourcef(_alSourceId, AL_CONE_OUTER_ANGLE, outerConeAngle);
	}

	void SoundSource::SetLoop(bool loop)
	{
		_isLoop = loop;
		if( _state == Playing )
			alSourcei(_alSourceId, AL_LOOPING, 
			loop ? AL_TRUE : AL_FALSE );
	}

	void SoundSource::SetIsSourceRelativeToListener(bool relative)
	{
		_relativeToListener = relative;
		if( _state == Playing )
			alSourcei(_alSourceId, AL_SOURCE_RELATIVE, relative);
	}

	void SoundSource::SetSourceValues()
	{
		alSourcef (_alSourceId, AL_PITCH, _pitch);
		alSourcef (_alSourceId, AL_GAIN, _gain);
		alSourcef (_alSourceId, AL_MAX_GAIN, 1.f);
		alSourcef (_alSourceId, AL_MIN_GAIN, 0.f);
		alSourcef (_alSourceId, AL_MAX_DISTANCE, _maxAttDistance);
		alSourcef (_alSourceId, AL_ROLLOFF_FACTOR, _rolloff);
		alSourcef (_alSourceId, AL_REFERENCE_DISTANCE, _halfDistance);
		alSourcef (_alSourceId, AL_CONE_OUTER_GAIN, _outerConeGain);
		alSourcef (_alSourceId, AL_CONE_INNER_ANGLE, _innerConeAngle);
		alSourcef (_alSourceId, AL_CONE_OUTER_ANGLE, _outerConeAngle);
		alSource3f(_alSourceId, AL_POSITION, _position.x, _position.y, _position.z);
		alSource3f(_alSourceId, AL_VELOCITY, _velocity.x, _velocity.y, _velocity.z);
		alSource3f(_alSourceId, AL_DIRECTION, _direction.x, _direction.y, _direction.z);
		alSourcei (_alSourceId, AL_SOURCE_RELATIVE, _relativeToListener);
		alSourcei (_alSourceId, AL_LOOPING, _isLoop);
		CheckALError(__FUNCTION__);
	}

	void SoundSource::CheckALError(const string& source)
	{
		int error = alGetError();

		if(error != AL_NO_ERROR)
		{
			CastException_d((string("OpenAL Error: ") + ALErrorToString(error) + "In function: " + source).c_str());
		}
	}

	string SoundSource::ALErrorToString(int error)
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
}