#pragma once

#include "ISelfActionFactory.h"
#include "SceneObjectSelfAction.h"
#include "IObjectRetriever.h"

namespace FlagRTS
{
	class SceneObjectMoveSelfAction : public SceneObjectSelfAction
	{
	private:
		IObjectRetriever<Vector3>* _move;

	public:
		SceneObjectMoveSelfAction(IObjectRetriever<Vector3>* move) :
			_move(move)
		{ }

		void operator()(SceneObject* so);
	};

	class SceneObjectMoveSelfActionFactory : public ISelfActionFactory<XmlNode*, SceneObject*>
	{
	public:
		SceneObjectMoveSelfActionFactory() { }
		~SceneObjectMoveSelfActionFactory() { }

		EventHandler<SceneObject*>* Create(XmlNode* actionNode);
	};
}