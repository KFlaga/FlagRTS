#include "FXSound.h"
#include <ISoundPlayer.h>
#include <SoundSource.h>

namespace FlagRTS
{
	FXSound::FXSound(FXSoundDefinition* fxDef) :
		FXObject(static_cast<FXObjectDefinition*>(fxDef)),
		_soundSource(0)
	{
		SetFinalType(GetTypeId<FXSound>());
		_soundSource = GetSoundPlayer()->RequestNewSource();
		// Set base parameters from definition
		_soundSource->SetGain( _soundSource->GetGain() * GetFXSoundDefinition()->GetGain() );

		_soundSource->SetHalfVolumeDistance( GetFXSoundDefinition()->GetHalfVolumeDistance() );
		_soundSource->SetMaxAttDistance( GetFXSoundDefinition()->GetMaxAttDistance() );
		_soundSource->SetRolloffFactor( GetFXSoundDefinition()->GetRolloffFactor() );

		_soundSource->SetIsSourceRelativeToListener(!GetFXSoundDefinition()->IsSceneSound());

		if( IsDirectional() )
		{
			_soundSource->SetInnerConeAngle( GetFXSoundDefinition()->GetInnerConeAngle() );
			_soundSource->SetOuterConeAngle( GetFXSoundDefinition()->GetOunterConeAngle() );
			_soundSource->SetOuterConeGain( GetFXSoundDefinition()->GetOunterConeGain() );
			_soundSource->SetDirection( GetFXSoundDefinition()->GetOrientationOffset() * Vector3::NEGATIVE_UNIT_Z );
		}
		_soundSource->SetPosition( GetFXSoundDefinition()->GetPositionOffset() );
		_playState = PlayState::Stopped;
	}

	FXSound::~FXSound()
	{
		if(_soundSource != 0)
			GetSoundPlayer()->ReleaseSource(_soundSource);
	}

	void FXSound::Update(float ms)
	{
		if( _playState == PlayState::Playing )
		{
			if( IsSceneSound() )
			{
				auto& pos = GetPositionAbsolute() + GetFXSoundDefinition()->GetPositionOffset();
				if( !_soundSource->GetPosition().positionEquals(pos, 0.1f) )
				{
					_soundSource->SetVelocity( (pos - _soundSource->GetPosition() ) / ms );
					_soundSource->SetPosition(pos);
				}
				else
					_soundSource->SetVelocity(Vector3(0.f,0.f,0.f));

				if( IsDirectional() )
				{
					auto& orient = GetOrientationAbsolute() * GetFXSoundDefinition()->GetOrientationOffset();
					Vector3 newDir = orient * Vector3(0.f,0.f,1.f);
					if( !newDir.directionEquals(_soundSource->GetDirection(), Radian(Degree(1.f))) )
					{
						_soundSource->SetDirection(newDir);
					}
				}
			}
		}
		FXObject::Update(ms);
	}

	void FXSound::Start()
	{
		_soundSource->SetPosition(GetPositionAbsolute() + 
			GetFXSoundDefinition()->GetPositionOffset());
		if( IsDirectional() )
		{
			auto orient = GetOrientationAbsolute() * GetFXSoundDefinition()->GetOrientationOffset();
			_soundSource->SetDirection( orient * Vector3(0.f,0.f,1.f) );
		}
		_progress = 0.f;
		_loopTimer = 0.f;

		GetSoundPlayer()->Play(_soundSource);
		_playState = PlayState::Playing;
	}

	void FXSound::Stop()
	{
		GetSoundPlayer()->Stop(_soundSource);
		_playState = PlayState::Stopped;
	}

	void FXSound::Pause()
	{
		GetSoundPlayer()->Pause(_soundSource);
		_playState = PlayState::Paused;
	}
}