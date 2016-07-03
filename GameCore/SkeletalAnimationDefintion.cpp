#include "SkeletalAnimationDefinition.h"
#include "SkeletalAnimation.h"

namespace FlagRTS
{
	SkeletalAnimationDefinition::SkeletalAnimationDefinition(XmlNode* animNode) :
		AnimationDefinition(animNode)
	{
		SetFinalType(GetTypeId<SkeletalAnimation>());
	}
}