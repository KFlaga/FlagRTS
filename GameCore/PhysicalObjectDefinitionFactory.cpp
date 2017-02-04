#include "MapDecoratorDefinition.h"
#include "UnitDefinition.h"
#include "MissleDefinition.h"

namespace FlagRTS
{
	PhysicalObjectDefinitionFactory::PhysicalObjectDefinitionFactory() :
		SubClassXmlFactory("PhysicalObjectDefiniton")
	{
		RegisterFactory("Unit", xNew0(FinalClassXmlFactory<UnitDefinition>));
		RegisterFactory("MapDecorator", xNew0(FinalClassXmlFactory<MapDecoratorDefinition>));
		RegisterFactory("Missle", xNew0(FinalClassXmlFactory<MissleDefinition>));
	}
}
