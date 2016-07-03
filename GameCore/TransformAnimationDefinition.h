#pragma once

#include "AnimationDefinition.h"
#include <Array.h>

namespace FlagRTS
{
	struct KeyFrame
	{
		float Length;
		Vector3 Translation;
		Vector3 Axis;
		Radian Angle;
	};

	class TransformAnimationDefinition : public AnimationDefinition
	{
	protected:
		Array<KeyFrame> _keyFrames;
		float _length;
	
	public:
		TransformAnimationDefinition();
		TransformAnimationDefinition(XmlNode* animNode);
	
		const Array<KeyFrame>& GetKeyFrames() const { return _keyFrames; }
	
		float GetLength() const { return _length; }
	};
}