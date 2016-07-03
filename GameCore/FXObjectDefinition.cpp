#include "FXObjectDefinition.h"
#include <Exception.h>

namespace FlagRTS
{
	FXObjectDefinition::FXObjectDefinition() :
		SceneObjectDefinition()
	{

	}

	FXObjectDefinition::FXObjectDefinition(XmlNode* node) :
		SceneObjectDefinition(node)
	{
		// Get node specific to PO
		XmlNode* poNode = node->first_node("FXObject");
		if(poNode != 0)
		{

			_effectName = XmlUtility::XmlGetString(poNode->first_node("Effect"), "name");
			_length = XmlUtility::XmlGetFloat(poNode->first_node("Length"), "value");

			_loop = XmlUtility::XmlGetBoolIfExists(poNode->first_node("Loop"),"value",false);
			_destroyOnEnd = XmlUtility::XmlGetBoolIfExists(poNode->first_node("DestroyOnEnd"),"value",true);
			_playOnSpawn = XmlUtility::XmlGetBoolIfExists(poNode->first_node("PlayOnSpawn"),"value",false);
		}
	}

	FXObjectDefinition::~FXObjectDefinition()
	{

	}
}