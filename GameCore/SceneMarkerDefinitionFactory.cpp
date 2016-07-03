#include "SceneMarkerDefinitionFactory.h"
#include "TerrainProjectionMarkerDefinition.h"
#include "ModelMarkerDefinition.h"
#include "BillboardMarkerDefinition.h"
#include <LogManager.h>
#include <Exception.h>

namespace FlagRTS
{
	DEFINE_FINAL_FACTORY(TerrainProjectionMarkerDefinitionFactory, 
		TerrainProjectionMarkerDefinition, 
		"TerrainProjectionMarkerDefinition", 
		XmlNode*);

	DEFINE_FINAL_FACTORY(ModelMarkerDefinitionFactory, 
		ModelMarkerDefinition, 
		"ModelMarkerDefinition", 
		XmlNode*);

	DEFINE_FINAL_FACTORY(BillboardMarkerDefinitionFactory, 
		BillboardMarkerDefinition, 
		"BillboardMarkerDefinition", 
		XmlNode*);

	SceneMarkerDefinitionFactory::SceneMarkerDefinitionFactory() :
		IFactoryCast("SceneMarkerDefinition")
	{
		_subtypeFactories.insert(std::make_pair("TerrainProjectionMarker",
			xNew0(TerrainProjectionMarkerDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair("ModelMarker",
			xNew0(ModelMarkerDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair("BillboardMarker",
			xNew0(BillboardMarkerDefinitionFactory)));
	}

	SceneMarkerDefinitionFactory::~SceneMarkerDefinitionFactory()
	{
		for(auto factoryIt = _subtypeFactories.begin(); factoryIt != _subtypeFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subtypeFactories.clear();
	}

	IGameObject* SceneMarkerDefinitionFactory::Create(XmlNode* objNode)
	{
		SceneMarkerDefinition* marker = 0;

		/*try
		{*/
			// First find higher level type of object and corretponding factory
			char* typeName = objNode->first_node("SceneMarker")->first_attribute("type", 4, false)->value();
			auto subFactory = _subtypeFactories.find(typeName);
			if(subFactory != _subtypeFactories.end()) // If subtype factory found
			{
				// Delegate creation of final object to subtype factory
				marker = (SceneMarkerDefinition*)subFactory->Value->Create(objNode);
			}
			else 
			{
				CastException((string("Bad SceneMarkerDefinition type : ") + typeName).c_str());
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

		return marker;
	}
}
