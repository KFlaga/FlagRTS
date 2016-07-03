#pragma once

#include "PhysicalObjectDefinition.h"

namespace FlagRTS
{
	class MapDecoratorDefinition : public PhysicalObjectDefinition
	{
	protected:

	public:
		MapDecoratorDefinition();
		MapDecoratorDefinition(XmlNode* soDefNode);
	};
}