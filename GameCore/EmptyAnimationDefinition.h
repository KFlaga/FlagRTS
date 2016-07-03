#pragma once

#include "AnimationDefinition.h"

namespace FlagRTS
{
	class EmptyAnimationDefinition : public AnimationDefinition
	{
	public:
		EmptyAnimationDefinition();
		EmptyAnimationDefinition(XmlNode* animNode);
	};
}