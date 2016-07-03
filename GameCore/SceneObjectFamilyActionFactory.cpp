#include "SceneObjectActionFamilyFactory.h"
#include "SceneObjectActions.h"
#include <Exception.h>

namespace FlagRTS
{

	SceneObjectFamilyActionFactory::SceneObjectFamilyActionFactory()
	{
		_factories["Move"] = xNew0(MoveAction::Factory);
		_factories["SetPosition"] = xNew0(SetPositionAction::Factory);
		_factories["Rotate"] = xNew0(RotateAction::Factory);
		_factories["SetOrientation"] = xNew0(SetOrientationAction::Factory);
		_factories["Spawn"] = xNew0(SpawnAction::Factory);
		_factories["Despawn"] = xNew0(DespawnAction::Factory);
	}

	SceneObjectFamilyActionFactory::~SceneObjectFamilyActionFactory()
	{
		for(auto facIt = _factories.begin(); facIt != _factories.end(); ++facIt)
		{
			xDelete(facIt->second);
		}
	}

	IAction* SceneObjectFamilyActionFactory::Create(XmlNode* actionNode)
	{
		// find action type and corresponding factory
		const char* actionType = XmlUtility::XmlGetString(actionNode,"type",4);
		auto factoryIt = _factories.find(actionType);
		if(factoryIt != _factories.end())
		{
			return factoryIt->second->Create(actionNode);
		}
		else
			CastException((string("Bad action type: ") + actionType).c_str());

		return 0;
	}
}