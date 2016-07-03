
#pragma once

#include <IFactory.h>
#include "SceneMarkerDefinition.h"
#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class SceneMarkerDefinitionFactory : public IFactoryCast<SceneMarkerDefinition, XmlNode*>
	{
	private:
		typedef ArrayMaps<IFactory<XmlNode*>*>::KeyString FactoryMap;
		FactoryMap _subtypeFactories;

	public:
		SceneMarkerDefinitionFactory();
		~SceneMarkerDefinitionFactory();

		IGameObject* Create(XmlNode* objectNode);
	};
}