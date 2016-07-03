#pragma once

#include <IFactory.h>
#include "IMoveStrategy.h"
#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class MoveStrategyFactory : public IFactoryCast<IMoveStrategy, XmlNode*>
	{
	private:
		typedef ArrayMaps<IFactory<XmlNode*>*>::KeyString FactoryMap;
		FactoryMap _subtypeFactories;

	public:
		MoveStrategyFactory();
		~MoveStrategyFactory();

		IGameObject* Create(XmlNode* objectNode);
	};
}