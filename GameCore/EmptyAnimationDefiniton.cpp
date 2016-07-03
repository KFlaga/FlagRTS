#include "EmptyAnimationDefinition.h"
#include "EmptyAnimation.h"

namespace FlagRTS
{
	EmptyAnimationDefinition::EmptyAnimationDefinition() :
		AnimationDefinition()
	{ 
		SetFinalType(GetTypeId<EmptyAnimation>());
	}

	EmptyAnimationDefinition::EmptyAnimationDefinition(XmlNode* animNode) :
		AnimationDefinition(animNode)
	{
		SetFinalType(GetTypeId<EmptyAnimation>());
	}
}