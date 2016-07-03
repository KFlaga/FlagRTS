
#pragma once

#include <IFactory.h>
#include "FXObjectDefinition.h"
#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class FXObjectDefinitionFactory : public IFactoryCast<FXObjectDefinition, XmlNode*>
	{
	private:
		typedef ArrayMaps<IFactory<XmlNode*>*>::KeyString FactoryMap;
		FactoryMap _subtypeFactories;

	public:
		FXObjectDefinitionFactory();
		~FXObjectDefinitionFactory();

		IGameObject* Create(XmlNode* objectNode);
	};
}