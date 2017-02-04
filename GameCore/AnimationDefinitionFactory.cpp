#include "AnimationDefinition.h"
#include "SkeletalAnimationDefinition.h"
#include "TransformAnimationDefinition.h"
#include "EmptyAnimationDefinition.h"

namespace FlagRTS
{
	AnimationDefinitionFactory::AnimationDefinitionFactory() :
		SubClassXmlFactory("AnimationDefinition")
	{
		RegisterFactory("Skeletal", xNew0(FinalClassXmlFactory<SkeletalAnimationDefinition>));
		RegisterFactory("Transform", xNew0(FinalClassXmlFactory<TransformAnimationDefinition>));
		RegisterFactory("Empty", xNew0(FinalClassXmlFactory<EmptyAnimationDefinition>));
	}
}