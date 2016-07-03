#include "TechRequirementFactory.h"
#include "ConditionFactory.h"
#include "DefaultTechReqs.h"
#include <Exception.h>

namespace FlagRTS
{
	TechRequirementFactory::TechRequirementFactory() :
		IFactoryCast("TechRequirement"),
		_conditionFactory(xNew0(ConditionFactory))
	{
		_defaultFactories["HaveAtLeastUnits"] = xNew0(PlayerHaveAtLeastUnitsOfKindRequirement::Factory);
		_defaultFactories["HaveNoMoreThanUnits"] = xNew0(PlayerHaveNoMoreThanUnitsOfKindRequirement::Factory);
		_defaultFactories["HaveExaclyUnits"] = xNew0(PlayerHaveExaclyUnitsOfKindRequirement::Factory);
		_defaultFactories["SpawnedAtLeastUnits"] = xNew0(PlayerSpawnedAtLeastUnitsOfKindRequirement::Factory);
		_defaultFactories["SpawnedNoMoreThanUnits"] = xNew0(PlayerSpawnedNoMoreThanUnitsOfKindRequirement::Factory);
		_defaultFactories["SpawnedExaclyUnits"] = xNew0(PlayerSpawnedExaclyUnitsOfKindRequirement::Factory);
	}

	TechRequirementFactory::~TechRequirementFactory()
	{
		xDelete(_conditionFactory);
		for(auto defFacIt = _defaultFactories.begin(); defFacIt != _defaultFactories.end(); ++defFacIt)
		{
			xDelete(defFacIt->Value);
		}
	}

	IGameObject* TechRequirementFactory::Create(XmlNode* reqNode)
	{
		TechRequirement* techReq = 0;
		bool isCustom = XmlUtility::XmlGetBoolIfExists(reqNode, "custom", false);
		if( isCustom )
		{
			// Create condition
		}
		else
		{
			// Create from default factories
			const char* type = XmlUtility::XmlGetStringIfExists(reqNode, "type", "");

			auto facIt = _defaultFactories.find(type);
			if( facIt != _defaultFactories.end() )
			{
				XmlNode* reqParamsNode = reqNode->first_node("Params");
				techReq = facIt->Value->CreateCast(reqParamsNode);
				techReq->SetName(XmlUtility::XmlGetString(reqNode, "name"));
			}
			else
			{
				CastException_d((string("Bad tech requirement type: ") + type).c_str());
				return 0;
			}

			XmlNode* hintNode = reqNode->first_node("Hint");
			if( hintNode != 0 )
			{
				const char* hint = XmlUtility::XmlGetLocaleString(hintNode).c_str();
				techReq->SetInavailableHint(hint);
			}
			else
			{
				techReq->SetInavailableHint("");
			}
		}

		return techReq;
	}
}