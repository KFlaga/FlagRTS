#include "AnimationDefinition.h"

namespace FlagRTS
{
	AnimationDefinition::AnimationDefinition()
	{ 
		_animName = "Default";
		_action = "Default";
		//_fadeIn = false;
		//_fadeOut = false;
		_speed = 1.0f;
		_loop = true;
	}

	AnimationDefinition::AnimationDefinition(XmlNode* animNode)
	{
		_animName = XmlUtility::XmlGetString(animNode, "name", 4);
		_action = XmlUtility::XmlGetString(animNode, "action", 6);
		//_fadeIn = XmlUtility::XmlGetBool(animNode, "fade_in", 7);
		//_fadeOut = XmlUtility::XmlGetBool(animNode, "fade_out", 8);
		_speed = XmlUtility::XmlGetFloatIfExists(animNode, "speed", 1.0f, 5);
		_loop = XmlUtility::XmlGetBoolIfExists(animNode, "loop", false);
	}
}