#pragma once

#include "PhysicalObject.h"

namespace Ogre
{
	class Material;
}

namespace FlagRTS
{
	// Copies unit's entity and sets outline material
	// Used mainly for choosing future builing location ( but may be usefull for some abilities as well )
	// Have 2 materials : Valid / Invalid ( probably location ), green and red one, half-transparent
	class ObjectOutline : public PhysicalObject
	{
		bool _isStateValid;
	public:
		ObjectOutline( PhysicalObjectDefinition* objectToBeCopied );
		~ObjectOutline();

		void Update(float ms) { }

		void Spawn();
		void Despawn();
		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		void SetIsStateValid(bool value);
		bool GetIsStateValid() const { return _isStateValid; }
	};
}