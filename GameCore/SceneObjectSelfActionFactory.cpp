#include "SceneObjectSelfActionFactory.h"
#include "SceneObjectSelfActions.h"

namespace FlagRTS
{
	SceneObjectSelfActionFactory::SceneObjectSelfActionFactory()
	{
		// add all self-actions
		AddFactory("Move", xNew0(SceneObjectMoveSelfActionFactory));
		// AddFactory("Rotate", xNew0(SceneObjectMoveSelfActionFactory));
		// AddFactory("Destroy", xNew0(SceneObjectMoveSelfActionFactory));
	}

	SceneObjectSelfActionFactory::~SceneObjectSelfActionFactory()
	{
		for(auto facIt = _subFactories.begin(); facIt != _subFactories.end(); ++facIt)
		{
			xDelete(facIt->second);
		}
		_subFactories.clear();
	}

	EventHandler<SceneObject*>* SceneObjectSelfActionFactory::Create(XmlNode* actionNode)
	{
		EventHandler<SceneObject*>* action = 0;
		// Find sub factory

		string actType = XmlUtility::XmlGetString(actionNode, "type", 4);
		auto subFactory = _subFactories.find(actType);
		if(subFactory != _subFactories.end())
		{
			action = subFactory->second->Create(actionNode);
		}

		return action;
	}
}