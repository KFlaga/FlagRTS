
#include "SceneObjectSelfEventFactory.h"
#include "MainEventPool.h"
#include "IAction.h"

namespace FlagRTS
{
	SceneObjectSelfEventFactory::SceneObjectSelfEventFactory()
	{

	}

	SceneObjectSelfEventFactory::~SceneObjectSelfEventFactory()
	{

	}


	Event<SceneObject*> SceneObjectSelfEventFactory::Create(XmlNode* eventNode)
	{
		Event<SceneObject*> targetEvent;

		// Acquire actions node
		XmlNode* actionListNode;
		if((actionListNode = eventNode->first_node("Actions",7)) != 0)
		{
			// Some action exists so parse all
			XmlNode* actionNode = actionListNode->first_node("Action",6);
			while(actionNode != 0)
			{
				bool queue = XmlUtility::XmlGetBool(actionNode->first_attribute("queue",5,false));

				string family = XmlUtility::XmlGetString(actionNode, "family", 6);
				if(family.compare("Self") == 0 
					|| family.compare("self") == 0)
				{
					targetEvent += _selfActionsFactory.Create(actionNode);
				}
				else
				{
					targetEvent += xNew1(ActionEventHandler<SceneObject*>,
						MainEventPool::GlobalPool->CreateAction(actionNode));
				}

				actionNode = actionNode->next_sibling("Action",6);
			}
		}

		return targetEvent;
	}
}