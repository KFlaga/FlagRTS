#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class SceneObject;

	// Base class capable of retrieveing SceneObject from various sources
	// Passes to actions.
	// Possible overloads : 
	// - global find by handle ( when stored in scripts local variable )
	// - triggering object ( for scene object events )
	// - last created unit
	// Retrievers are owned by actions and deleted in destructors
	// Sould have overloaded operator SceneObject*() ( and actually nothing more )
	typedef IObjectRetriever<SceneObject*> ISceneObjectRetriever;

	class SceneObjectRetrieverFactory
	{
	public:
		static ISceneObjectRetriever* Create(XmlNode* xml);
	};
}