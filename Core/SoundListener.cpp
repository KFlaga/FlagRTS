#include "SoundListener.h"

namespace FlagRTS
{
	using namespace Media;

	SoundListener::SoundListener()
	{

	}

	SoundListener::~SoundListener()
	{

	}

	void SoundListener::SetPosition(const Vector3& vec)
	{
		_position = vec;
		_transformChanged = true;
	}

	const Vector3& SoundListener::GetPosition() const
	{
		return _position;
	}

	void SoundListener::SetDirection(const Vector3& vec)
	{
		_direction = vec;
		_transformChanged = true;
	}

	const Vector3& SoundListener::GetDirection() const
	{
		return _direction;
	}

	void SoundListener::SetUp(const Vector3& vec)
	{
		_up = vec;
		_transformChanged = true;
	}

	const Vector3& SoundListener::GetUp() const
	{
		return _up;
	}

	void SoundListener::SetVelocity(const Vector3& vec)
	{
		_velocity = vec;
		alListener3f(AL_VELOCITY, vec.x, vec.y, vec.z);
	}

	const Vector3& SoundListener::GetVelocity() const
	{
		return _velocity;
	}

	void SoundListener::SetGain(float gain)
	{
		alListenerf (AL_GAIN, gain);
	}

	void SoundListener::Update(float ms)
	{
		if( _transformChanged)
		{
			alListener3f(AL_POSITION, _position.x, _position.y, _position.z);

			_orientation[0]= -_direction.x;
			_orientation[1]= -_direction.y;
			_orientation[2]= -_direction.z;

			_orientation[3]= _up.x;
			_orientation[4]= _up.y;
			_orientation[5]= _up.z;
			alListenerfv(AL_ORIENTATION, _orientation); 
			_transformChanged = false;
		}
	}

	void SoundListener::ResetValues()
	{
		_position = Vector3(0.f,0.f,0.f);
		_direction = Vector3(0.f,0.f,-1.f);
		_up = Vector3(0.f,1.f,0.f);
		_velocity = Vector3(0.f,0.f,0.f);

		_orientation[0]= _direction.x; // Forward.x
		_orientation[1]= _direction.y; // Forward.y
		_orientation[2]= _direction.z; // Forward.z

		_orientation[3]= _up.x; // Up.x
		_orientation[4]= _up.y; // Up.y
		_orientation[5]= _up.z; // Up.z

		alListener3f(AL_POSITION, _position.x, _position.y, _position.z);
		alListenerfv(AL_ORIENTATION, _orientation);
		alListenerf (AL_GAIN, 0.6f);
		alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	}
}