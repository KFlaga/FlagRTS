#include "SoundChannel.h"
#include "SoundSource.h"
#include "AudioManager.h"

namespace FlagRTS
{
	using namespace Media;

	SoundChannel::SoundChannel(int id) :
		_id(id),
		_currentSource(0),
		_gain(1.f)
	{

	}

	void SoundChannel::Play(SoundSource* ssource, bool stopCurrent)
	{
		_ASSERT(ssource != 0);
		if( ssource == 0 )
			return;

		if( _currentSource != 0 )
		{
			if( _currentSource->GetCurrentState() == Playing )
			{
				if( stopCurrent == false )
					return;

				AudioManager::Instance()->StopSoundSource(_currentSource);
			}
			else if( _currentSource->GetCurrentState() == Paused )
			{
				AudioManager::Instance()->StopSoundSource(_currentSource);
			}
		}

		_currentSource = ssource;
		_currentSource->SetGain( _currentSource->GetGain() * _gain );
		AudioManager::Instance()->PlaySoundSource(_currentSource);
	}

	void SoundChannel::Pause(SoundSource* ssource)
	{
		_ASSERT( _currentSource != 0 );
		if( ssource == _currentSource && ssource != 0 )
		{
			AudioManager::Instance()->PauseSoundSource(ssource);
		}
	}

	void SoundChannel::Stop(SoundSource* ssource)
	{
		_ASSERT( _currentSource != 0 );
		if( ssource == _currentSource && ssource != 0 )
		{
			_currentSource->SetGain( _currentSource->GetGain() / _gain );
			AudioManager::Instance()->StopSoundSource(ssource);
		}
	}

	bool SoundChannel::IsPlaying() const
	{
		return _currentSource != 0 &&
			_currentSource->GetCurrentState() == Playing;
	}

	void SoundChannel::SetGain(float value)
	{
		if( _currentSource != 0 &&
			(_currentSource->GetCurrentState() == Playing || 
			 _currentSource->GetCurrentState() == Paused) )
		{
			_currentSource->SetGain( _currentSource->GetGain() / _gain );
			_currentSource->SetGain( _currentSource->GetGain() * value );
		}
		_gain = value;
	}

}