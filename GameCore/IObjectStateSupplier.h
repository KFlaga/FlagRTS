#pragma once

#include <TypeDefs.h>

namespace FlagRTS
{
	class SceneObjectState;
	class SceneObject;

	// Adds SceneObjectStates to given SceneObject
	// Per object class/type one derived class should be created
	class IObjectStateSupplier
	{
	public:
		virtual void AddStatesToObject(SceneObject* targetObject) = 0;
	};
}