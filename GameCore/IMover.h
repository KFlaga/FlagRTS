#pragma once

#include "ObjectDefinition.h"
#include "GameObjectFactory.h"

namespace FlagRTS
{
	class SceneObject;

	// Interface for mover objects that just moves SceneObject in some 
	// predefined way. They need current movement time to compute move in current frame.
	// They don't track object movement and so same mover may be used for all
	// objects of same type, but also movement end condition should be check by mover's user
	// Movers follow same path every time ( only transformed by object position/orientation ),
	// so best used for missles. 
	// All this also means that using Mover should be only way object moves
	// at any moment or path will be invalid ( as mover assumes this requirement is met )
	class IMover : public ObjectDefinition
	{
	public:
		IMover() { }
		IMover(XmlNode* node) :
			ObjectDefinition(node)
		{ }

		virtual ~IMover() { }
		// Moves object and returns distance traveled in this frame
		virtual float Move(SceneObject* object, float moveTime, float frameTime) = 0;
	};

	class MoverFactory : public SubClassXmlFactory
	{
	public:
		MoverFactory();
	};
}