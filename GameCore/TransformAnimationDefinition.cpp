#include "TransformAnimationDefinition.h"
#include "TransformAnimation.h"

namespace FlagRTS
{
	TransformAnimationDefinition::TransformAnimationDefinition(XmlNode* animNode) :
		AnimationDefinition(animNode)
	{
		SetFinalType(GetTypeId<TransformAnimation>());
		
		// Parse keyframes
		_length = 0.f;
		XmlNode* frameNode = animNode->first_node("KeyFrame");
		while(frameNode != 0)
		{
			KeyFrame frame;
			frame.Length = XmlUtility::XmlGetFloat(frameNode, "length", 6);
			frame.Translation = XmlUtility::XmlGetXYZ(frameNode->first_node("Translation"));
			frame.Axis = XmlUtility::XmlGetXYZ(frameNode->first_node("Rotation"));
			frame.Angle = Degree(XmlUtility::XmlGetFloat(frameNode->first_node("Rotation"), "a", 1));
			_keyFrames.push_back(frame);

			_length += frame.Length;
			frameNode = frameNode->next_sibling();
		}
	}
}