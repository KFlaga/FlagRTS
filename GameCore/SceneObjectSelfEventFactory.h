#pragma once

#include "SceneObjectSelfActionFactory.h"
#include "SceneObject.h"
#include <map>

namespace FlagRTS
{
	class SceneObjectSelfEventFactory
	{
	private:
		SceneObjectSelfActionFactory _selfActionsFactory;

	public:
		SceneObjectSelfEventFactory();
		~SceneObjectSelfEventFactory();

		Event<SceneObject*> Create(XmlNode* eventNode);
	};
}