#include "SceneObjectDefinitionFactory.h"
#include "PhysicalObjectDefinitionFactory.h"
#include "GameCameraDefinition.h"
#include "SceneMarkerDefinitionFactory.h"
#include "FXObjectDefinitionFactory.h"
#include <LogManager.h>
#include <Exception.h>

namespace FlagRTS
{
	DEFINE_FINAL_FACTORY(GameCameraDefinitionFactory, 
		GameCameraDefinition, 
		"GameCameraDefinition", 
		XmlNode*);

	SceneObjectDefinitionFactory::SceneObjectDefinitionFactory() :
		IFactoryCast("SceneObjectDefiniton")
	{
		_subtypeFactories.insert(std::make_pair("PhysicalObject",
			xNew0(PhysicalObjectDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair("GameCamera",
			xNew0(GameCameraDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair("SceneMarker",
			xNew0(SceneMarkerDefinitionFactory)));
		_subtypeFactories.insert(std::make_pair("FXObject",
			xNew0(FXObjectDefinitionFactory)));
	}

	SceneObjectDefinitionFactory::~SceneObjectDefinitionFactory()
	{
		for(auto factoryIt = _subtypeFactories.begin(); factoryIt != _subtypeFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subtypeFactories.clear();
	}

	IGameObject* SceneObjectDefinitionFactory::Create(XmlNode* objNode)
	{
		SceneObjectDefinition* obj = 0;

		// First find higher level type of object and corretponding factory
		char* typeName = objNode->first_attribute("type", 4, false)->value();
		auto subFactory = _subtypeFactories.find(typeName);
		if(subFactory != _subtypeFactories.end()) // If subtype factory found
		{
			// Delegate creation of final object to subtype factory
			obj = (SceneObjectDefinition*)subFactory->Value->Create(objNode);
		}
		else
		{
			CastException((string("Bad SceneObjectDefinition type : ") + typeName).c_str());
		}

		return obj;
	}
}
