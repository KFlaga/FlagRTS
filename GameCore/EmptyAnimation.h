#pragma once

#include "EmptyAnimationDefinition.h"
#include "Animation.h"

namespace FlagRTS
{
	class EmptyAnimation : public Animation
	{
	public:
		// animNode is named "Animation"
		EmptyAnimation(EmptyAnimationDefinition* animDef) :
			Animation(animDef)
		{ 
			_animLength = 1000.f;
			_animProgress = 1.0f;
		}

		void Start() { }
		void End() { }
		void Update(float ms) { }
	};
		
}