#pragma once

#include "AnimationDefinition.h"
#include <OgreAnimationState.h>
#include <list>

namespace FlagRTS
{
	class SkeletalAnimationDefinition : public AnimationDefinition
	{
	public:
		SkeletalAnimationDefinition();
		SkeletalAnimationDefinition(XmlNode* animNode);
	};
}