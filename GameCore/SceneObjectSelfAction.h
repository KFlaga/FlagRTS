#pragma once

#include <Event.h>
#include "SceneObject.h"

namespace FlagRTS
{
	// Base class for all actions that operates on SceneObjects
	// but given via event parameter
	class SceneObjectSelfAction : public EventHandler<SceneObject*>
	{
	public:
		SceneObjectSelfAction() { }
	};
}