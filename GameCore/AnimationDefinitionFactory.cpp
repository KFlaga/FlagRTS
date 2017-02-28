#include "AnimationDefinition.h"
#include "SkeletalAnimationDefinition.h"
#include "TransformAnimationDefinition.h"
#include "EmptyAnimationDefinition.h"

namespace FlagRTS
{
	AnimationDefinitionFactory::AnimationDefinitionFactory()
	{
		RegisterFactory("Skeletal", xNew0(FinalObjectDefinitionFactory<SkeletalAnimationDefinition>));
		RegisterFactory("Transform", xNew0(FinalObjectDefinitionFactory<TransformAnimationDefinition>));
		RegisterFactory("Empty", xNew0(FinalObjectDefinitionFactory<EmptyAnimationDefinition>));
	}
}