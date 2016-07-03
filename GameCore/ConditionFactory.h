#pragma once

#include "ICondition.h"
#include <IFactory.h>
#include <Xml.h>
#include <HashMap.h>

namespace FlagRTS
{
	// Creates requirements based on Xml
	// Creates predefined requirements or ones using generic ICondition created in IConditionFactory
	class ConditionFactory : public IConditionFactory
	{
		HashedMaps<IConditionFactory*>::KeyCCString _condFamilyFactoryies;

	public:
		ConditionFactory();
		~ConditionFactory();

		ICondition* Create(XmlNode* condNode);
	};
}