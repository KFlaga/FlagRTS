#include "SceneObjectSelfActions.h"
#include "Vector3Retrievers.h"

namespace FlagRTS
{
	void SceneObjectMoveSelfAction::operator()(SceneObject* so)
	{
		so->Move(*_move);
	}

	EventHandler<SceneObject*>* SceneObjectMoveSelfActionFactory::Create(XmlNode* actionNode)
	{
		IObjectRetriever<Vector3>* move = 
			Vector3RetrieverFactory::Create(actionNode->first_node("Move",4));

		// create move action with obtained vector
		return xNew1(SceneObjectMoveSelfAction,move);
	}
}