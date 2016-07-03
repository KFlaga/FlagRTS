#include "FXObjectDefinitionFactory.h"
#include "FXSoundDefinition.h"
#include "FXParticlesDefinition.h"
#include <LogManager.h>
#include <Exception.h>

namespace FlagRTS
{
	DEFINE_FINAL_FACTORY(FXSoundDefinitionFactory, 
		FXSoundDefinition, 
		"FXSoundDefinition", 
		XmlNode*);

	DEFINE_FINAL_FACTORY(FXParticlesDefinitionFactory, 
		FXParticlesDefinition, 
		"FXParticlesDefinition", 
		XmlNode*);

	FXObjectDefinitionFactory::FXObjectDefinitionFactory() :
		IFactoryCast("FXObjectDefinition")
	{
		_subtypeFactories.insert(std::make_pair("FXSound",
			xNew0(FXSoundDefinitionFactory)));

		_subtypeFactories.insert(std::make_pair("FXParticles",
			xNew0(FXParticlesDefinitionFactory)));
	}

	FXObjectDefinitionFactory::~FXObjectDefinitionFactory()
	{
		for(auto factoryIt = _subtypeFactories.begin(); factoryIt != _subtypeFactories.end(); factoryIt++)
		{
			xDelete(factoryIt->Value);
		}
		_subtypeFactories.clear();
	}

	IGameObject* FXObjectDefinitionFactory::Create(XmlNode* objNode)
	{
		FXObjectDefinition* fxobj = 0;

		// First find higher level type of object and corretponding factory
		char* typeName = objNode->first_node("FXObject")->first_attribute("type", 4, false)->value();
		auto subFactory = _subtypeFactories.find(typeName);
		if(subFactory != _subtypeFactories.end()) // If subtype factory found
		{
			// Delegate creation of final object to subtype factory
			fxobj = (FXObjectDefinition*)subFactory->Value->Create(objNode);
		}
		else 
		{
			CastException((string("Bad FXObjectDefinition type : ") + typeName).c_str());
		}

		return fxobj;
	}
}
