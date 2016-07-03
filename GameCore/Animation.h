#pragma once

#include "AnimationDefinition.h"
#include <ArrayMap.h>

namespace FlagRTS
{
	class Animation : public IGameObject
	{
	protected:
		AnimationDefinition* _animDef;
		float _animLength;
		float _animProgress;
		bool _loop;

	public:
		// animNode is named "Animation"
		Animation(AnimationDefinition* animDef) :
			_animDef(animDef),
			_animLength(0.0f),
			_animProgress(0.0f),
			_loop(animDef->GetLoop())
		{ }

		virtual ~Animation() { }

		const string& GetName() const { return _animDef->GetName(); }
		const string& GetAction() const { return _animDef->GetAction(); }
		bool GetFadeIn() const { return _animDef->GetFadeIn(); }
		bool GetFadeOut() const { return _animDef->GetFadeOut(); }
		TypeId GetType() const { return _animDef->GetFinalType(); }

		virtual bool GetLoop() const { return _loop; }
		virtual void SetLoop(bool value) 
		{ 
			_loop = value; 
		}

		bool CheckIsFinished()
		{
			return _animProgress > 0.9999f;
		}

		virtual void Start() = 0;
		virtual void End() = 0;
		virtual void Update(float ms) = 0;

		float GetLength() const { return _animLength; }
		// Returns part of animation that is already done ( in range [0-1] )
		float GetProgress() const { return _animProgress; }
	};

	typedef ArrayMap<string, Animation*, StringIsEqual> AnimationSet;
	typedef ArrayMap<int, AnimationSet*> AnimationMap;
		
}