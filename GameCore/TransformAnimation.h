#pragma once

#include "Animation.h"
#include "TransformAnimationDefinition.h"


namespace FlagRTS
{
	class SceneObject;

	class TransformAnimation : public Animation
	{
	protected:
		SceneObject* _owner;
		int _currentKeyFrame;
		float _currentMs;

	public:
		TransformAnimation(TransformAnimationDefinition* taDef, SceneObject* owner);
		~TransformAnimation();

		void Start();
		void End();
		void Update(float ms);

		void SetLoop(bool value);
		
		const Array<KeyFrame>& GetKeyFrames() const 
		{ 
			return static_cast<TransformAnimationDefinition*>(_animDef)->GetKeyFrames(); 
		}
	};
}