#pragma once

#include "SceneObject.h"
#include "FXObjectDefinition.h"

namespace FlagRTS
{
	// FXs are all kinds of effects, placeable on scene including sounds, particles,
	// ligths etc. Most of them are by rule not independent and should be attached to
	// other SceneObjects ( with exception of FXEmitter )
	// Provide simple control interface : start / pause / end / isfinished etc
	class FXObject : public SceneObject
	{
	public:
		enum PlayState : int
		{
			Stopped,
			Playing,
			Paused,
			Waiting,
		};

	protected:
		FXObjectDefinition* _fxDef;
		float _playSpeed; // Play speed multilpier ( used if applicable )
		PlayState _playState;
		float _progress; // Effect progress, when looped may be > 1
		float _loopTimer; // Same as progress but resets on each loop
		bool _looped; // Effect is looped if applicable

	public:
		FXObject(FXObjectDefinition* poDef);
		virtual ~FXObject();

		FXObjectDefinition* GetFXObjectDefinition() const
		{ 
			return _fxDef; 
		}

		PlayState GetCurrentPlayState() const { return _playState; }

		// FXObjects dont use SceneObjectStates, so it wont call SceneObject::Update() 
		// ( they can be playing/stopped/paused only )
		// Updates timer if state = playing and if it reached length of effect, then:
		// - if DestroyOnEnd is true -> stops fx and queue its despawn/destruction
		// - if Loop == true, then calls Reset()
		// - if Loop == false, then Stops fx 
		void Update(float ms);

		// Starts fx if PlayOnStart() == true
		// Should be called after derived spawn
		void Spawn();
		// Stops effect if not stopped
		void Despawn();

		virtual void Start() = 0;
		virtual void Stop() = 0;
		virtual void Pause() { }
		virtual void Reset()
		{
			_loopTimer = 0.f;
		}

		bool IsFinished() const { return _progress >= 1.f; }
		float GetProgress() const { return _progress; }
		virtual void SetProgress(float value) 
		{ 
			_progress = value; 
			_loopTimer = value;
		}

		float GetLoopProgress() const { return _loopTimer; }

		virtual void SetIsLooped(bool value) { _looped = value; }
		bool IsLooped() const { return _looped; }

		float GetLength() const { return _fxDef->GetLength(); }
		const string& GetEffectName() const { return _fxDef->GetEffectName(); }

		virtual void SetPlaySpeed(float value) { _playSpeed = value; }
		float GetPlaySpeed() const { return _playSpeed; }
	};
}