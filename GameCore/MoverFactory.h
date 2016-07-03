#pragma once

#include <IFactory.h>
#include "IMover.h"
#include <Xml.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class MoverFactory : public IFactoryCast<IMover, XmlNode*>
	{
	private:
		typedef ArrayMaps<IFactory<XmlNode*>*>::KeyString FactoryMap;
		FactoryMap _subtypeFactories;

	public:
		MoverFactory();
		~MoverFactory();

		IGameObject* Create(XmlNode* objectNode);
	};
}