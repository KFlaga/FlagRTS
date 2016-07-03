#pragma once

#include <IFactory.h>
#include "IEffect.h"
#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class EffectFactory : public IFactoryCast<IEffect, XmlNode*>
	{
	private:
		typedef ArrayMaps<IFactory<XmlNode*>*>::KeyString FactoryMap;
		FactoryMap _subtypeFactories;

	public:
		EffectFactory();
		~EffectFactory();

		IGameObject* Create(XmlNode* objectNode);
	};
}