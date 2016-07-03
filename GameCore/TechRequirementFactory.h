#pragma once

#include "TechRequirement.h"
#include <IFactory.h>
#include <Xml.h>
#include <HashMap.h>

namespace FlagRTS
{
	struct IConditionFactory;

	// Creates requirements based on Xml
	// Creates predefined requirements or ones using generic ICondition created in IConditionFactory
	// Creating TechRequirements requires loaded ObjectDefinitions
	class TechRequirementFactory : public IFactoryCast<TechRequirement, XmlNode*>
	{
		IConditionFactory* _conditionFactory;
		HashedMaps<IFactoryCast<TechRequirement, XmlNode*>*>::KeyCCString _defaultFactories;

	public:
		TechRequirementFactory();
		~TechRequirementFactory();

		IGameObject* Create(XmlNode* reqNode);
	};
}