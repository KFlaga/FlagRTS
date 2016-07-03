
#pragma once

#include <IFactory.h>
#include "SceneObjectDefinition.h"
#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	// Factory for generic scene objects definitions
	// Loads SO info from xml file
	// Calls specialized factory for creation of specific type SceneObjectDefs
	// Sets properties common for every SceneObject of givrn type
	class SceneObjectDefinitionFactory : public IFactoryCast<SceneObjectDefinition, XmlNode*>
	{
	private:
		typedef ArrayMaps<IFactory<XmlNode*>*>::KeyString FactoryMap;
		FactoryMap _subtypeFactories;

	public:
		SceneObjectDefinitionFactory();
		~SceneObjectDefinitionFactory();

		IGameObject* Create(XmlNode* objectNode);
	};
}