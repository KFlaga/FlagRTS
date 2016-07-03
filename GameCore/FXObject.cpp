#include "FXObject.h"
#include "GameWorld.h"

namespace FlagRTS
{
	FXObject::FXObject(FXObjectDefinition* fxDef) :
		SceneObject(static_cast<SceneObjectDefinition*>(fxDef)),
		_fxDef(static_cast<FXObjectDefinition*>(_definition)),
		_playSpeed(1.f),
		_playState(PlayState::Stopped),
		_progress(0.f),
		_looped(_fxDef->GetLoop())
	{

	}

	FXObject::~FXObject()
	{

	}

	void FXObject::Update(float ms)
	{
		if( _playState == PlayState::Playing )
		{
			float addTime = (ms*_playSpeed) / _fxDef->GetLength();
			_progress += addTime;
			_loopTimer += addTime;
			if( _loopTimer >= 1.f )
			{
				if( _fxDef->GetDestroyOnEnd() )
				{
					Stop();
					GameWorld::GlobalWorld->QueueDespawnSceneObject(this);
					GameWorld::GlobalWorld->QueueDestroySceneObject(this);
				}
				else if( _looped == true )
					Reset();
				else
					Stop();
			}
		}
	}

	void FXObject::Spawn()
	{
		SceneObject::Spawn();
		if(_fxDef->GetPlayOnSpawn())
		{
			Start();
		}
	}

	void FXObject::Despawn()
	{
		if(_playState != PlayState::Stopped)
			Stop();
		SceneObject::Despawn();
	}
}