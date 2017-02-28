#include "MapDecoratorDefinition.h"
#include "MapDecorator.h"
#include "SelectionFlags.h"

namespace FlagRTS
{
	MapDecoratorDefinition::MapDecoratorDefinition() :
		PhysicalObjectDefinition()
	{
		SetFinalType(GetTypeId<MapDecorator>());
		SetFinalTypeName("MapDecorator");
	}

	MapDecoratorDefinition::MapDecoratorDefinition(XmlNode* soDefNode) :
		PhysicalObjectDefinition(soDefNode)
	{
		SetFinalType(GetTypeId<MapDecorator>());
		SetFinalTypeName("MapDecorator");

		// Get node specific to MapDecorator
		XmlNode* mdNode = soDefNode->first_node("PhysicalObject")->
			first_node("MapDecorator");
	}
}