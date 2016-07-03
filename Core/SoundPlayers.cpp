#include "SoundPlayers.h"
#include "SoundSource.h"
#include "Sound.h"
#include "AudioManager.h"

namespace FlagRTS
{
	using namespace Media;

	SingleSoundPlayer::SingleSoundPlayer() :
		ISoundPlayer(),
		_currentSource(0)
	{

	}

	SingleSoundPlayer::SingleSoundPlayer(const char* soundName, const char* soundPack) :
		ISoundPlayer(soundName, soundPack),
		_currentSource(0)
	{

	}

	SingleSoundPlayer::SingleSoundPlayer(Sound* sound) :
		ISoundPlayer(sound),
		_currentSource(0)
	{

	}

	void SingleSoundPlayer::Play(SoundSource* ssource)
	{
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		if( _currentSource != 0 && 
			_currentSource->GetCurrentState() != Unloaded)
		{
			AudioManager::Instance()->StopSoundSource(_currentSource);
		}
		_currentSource = ssource;
		AudioManager::Instance()->PlaySoundSource( ssource );
	}

	void SingleSoundPlayer::Pause(SoundSource* ssource)
	{
		if( _currentSource != ssource )
			return;
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		AudioManager::Instance()->PauseSoundSource( ssource );
	}

	void SingleSoundPlayer::Stop(SoundSource* ssource)
	{
		if( _currentSource != ssource )
			return;
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		AudioManager::Instance()->StopSoundSource( ssource );
		_currentSource = 0;
	}

	SingleNoStopSoundPlayer::SingleNoStopSoundPlayer() :
		ISoundPlayer(),
		_currentSource(0)
	{

	}

	SingleNoStopSoundPlayer::SingleNoStopSoundPlayer(const char* soundName, const char* soundPack) :
		ISoundPlayer(soundName, soundPack),
		_currentSource(0)
	{

	}

	SingleNoStopSoundPlayer::SingleNoStopSoundPlayer(Sound* sound) :
		ISoundPlayer(sound),
		_currentSource(0)
	{

	}

	void SingleNoStopSoundPlayer::Play(SoundSource* ssource)
	{
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		if( _currentSource != 0 && 
			_currentSource->GetCurrentState() != Unloaded)
		{
			return;
		}
		_currentSource = ssource;
		AudioManager::Instance()->PlaySoundSource( ssource );
	}

	void SingleNoStopSoundPlayer::Pause(SoundSource* ssource)
	{
		if( _currentSource != ssource )
			return;
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		AudioManager::Instance()->PauseSoundSource( ssource );
	}

	void SingleNoStopSoundPlayer::Stop(SoundSource* ssource)
	{
		if( _currentSource != ssource )
			return;
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		AudioManager::Instance()->StopSoundSource( ssource );
		_currentSource = 0;
	}

	MusicSoundPlayer::MusicSoundPlayer() :
		ISoundPlayer(),
		_currentSource(0)
	{

	}

	MusicSoundPlayer::MusicSoundPlayer(const char* soundName, const char* soundPack) :
		ISoundPlayer(soundName, soundPack),
		_currentSource(0)
	{

	}

	MusicSoundPlayer::MusicSoundPlayer(Sound* sound) :
		ISoundPlayer(sound),
		_currentSource(0)
	{

	}

	void MusicSoundPlayer::Play(SoundSource* ssource)
	{
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		_ASSERT( ssource == _currentSource );
		_currentSource = ssource;
		AudioManager::Instance()->PlaySoundSource( ssource );
	}

	void MusicSoundPlayer::Pause(SoundSource* ssource)
	{
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		_ASSERT( ssource == _currentSource );
		AudioManager::Instance()->PauseSoundSource( ssource );
	}

	void MusicSoundPlayer::Stop(SoundSource* ssource)
	{
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		_ASSERT( ssource == _currentSource );
		AudioManager::Instance()->StopSoundSource( ssource );
		_currentSource = 0;
	}

	SoundSource* MusicSoundPlayer::RequestNewSource()
	{
		if(_currentSource == 0)
		{
			_currentSource = AudioManager::Instance()->CreateSoundSource(_sound);
			_allSources.push_back(_currentSource);
		}
		return _currentSource;
	}

	RevolverSoundPlayer::RevolverSoundPlayer(int maxPlays) :
		ISoundPlayer(),
		_nextSource(0),
		_maxPlays(maxPlays)
	{ 
		_sources.resize(maxPlays, 0);
	}

	RevolverSoundPlayer::RevolverSoundPlayer(const char* soundName, 
		const char* soundPack,
		int maxPlays) :
		ISoundPlayer(soundName, soundPack),
		_nextSource(0),
		_maxPlays(maxPlays)
	{ 
		_sources.resize(maxPlays, 0);
	}

	RevolverSoundPlayer::RevolverSoundPlayer(Sound* sound,
		int maxPlays) :
		ISoundPlayer(sound),
		_nextSource(0),
		_maxPlays(maxPlays)
	{ 
		_sources.resize(maxPlays, 0);
	}

	void RevolverSoundPlayer::Play(SoundSource* ssource)
	{
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		auto audio = AudioManager::Instance();
		if( _sources[_nextSource] != 0 && 
			(_sources[_nextSource]->GetCurrentState() != Unloaded || 
			_sources[_nextSource]->GetCurrentState() != Stopped))
		{
			audio->StopSoundSource(_sources[_nextSource]);
		}
		_sources[_nextSource] = ssource;
		audio->PlaySoundSource( _sources[_nextSource] );
		_nextSource = _nextSource < _maxPlays - 1 ? _nextSource + 1 : 0;
	}

	void RevolverSoundPlayer::Pause(SoundSource* ssource)
	{
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		AudioManager::Instance()->PauseSoundSource( ssource );
	}

	void RevolverSoundPlayer::Stop(SoundSource* ssource)
	{
		_ASSERT( ssource != 0 );
		_ASSERT( _sound != 0 );
		if(ssource->GetCurrentState() == Playing ||
			ssource->GetCurrentState() == Paused )
		{
			AudioManager::Instance()->StopSoundSource( ssource );
		}
	}
}
