#include "PhysicalObjectDefinitionFactory.h"
#include "MapDecoratorDefinition.h"
#include "UnitDefinition.h"
#include "MissleDefinition.h"
#include <LogManager.h>
#include <Exception.h>

namespace FlagRTS
{
	DEFINE_FINAL_FACTORY(MapDecoratorDefinitionFactory, 
		MapDecoratorDefinition, 
		"MapDecoratorDefinition", 
		XmlNode*);

	DEFINE_FINAL_FACTORY(UnitDefinitionFactory, 
		UnitDefinition, 
		"UnitDefinition", 
		XmlNode*);
	
	DEFINE_FINAL_FACTORY(MissleDefinitionFactory, 
		MissleDefinition, 
		"MissleDefinition", 
		XmlNode*);

	PhysicalObjectDefinitionFactory::PhysicalObjectDefinitionFactory() :
		IFactoryCast("PhysicalObjectDefiniton")
	{
		_subtypeFactories.insert(std::make_pair("MapDecorator",
			xNew0(MapDecoratorDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair("Unit",
			xNew0(UnitDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair("Missle",
			xNew0(MissleDefinitionFactory)));
	}

	PhysicalObjectDefinitionFactory::~PhysicalObjectDefinitionFactory()
	{
		for(auto factoryIt = _subtypeFactories.begin(); factoryIt != _subtypeFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subtypeFactories.clear();
	}

	IGameObject* PhysicalObjectDefinitionFactory::Create(XmlNode* objNode)
	{
		PhysicalObjectDefinition* pobj = 0;

		/*try
		{*/
			// First find higher level type of object and corretponding factory
			char* typeName = objNode->first_node("PhysicalObject")->first_attribute("type", 4, false)->value();
			auto subFactory = _subtypeFactories.find(typeName);
			if(subFactory != _subtypeFactories.end()) // If subtype factory found
			{
				// Delegate creation of final object to subtype factory
				pobj = (PhysicalObjectDefinition*)subFactory->Value->Create(objNode);
			}
			else 
			{
				CastException((string("Bad PhysicalObjectDefinition type : ") + typeName).c_str());
			}
		//}
		//catch(...) // Failed to create object - whaterver the reasons, just return invalid pointer
		//{
		//	pobj = 0;
		//	if(objNode != 0)
		//		ADD_ERRLOG("Failed to parse physical object definition node");
		//	else
		//		ADD_ERRLOG("Passed nullptr to PhysicalObjectDefinition factory");
		//}

		return pobj;
	}
}
