
#pragma once

#include <IFactory.h>
#include "PhysicalObjectDefinition.h"
#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class PhysicalObjectDefinitionFactory : public IFactoryCast<PhysicalObjectDefinition, XmlNode*>
	{
	private:
		typedef ArrayMaps<IFactory<XmlNode*>*>::KeyString FactoryMap;
		FactoryMap _subtypeFactories;

	public:
		PhysicalObjectDefinitionFactory();
		~PhysicalObjectDefinitionFactory();

		IGameObject* Create(XmlNode* objectNode);
	};
}