#pragma once

#include "PhysicalObject.h"
#include "MapDecoratorDefinition.h"

namespace FlagRTS
{
	// Map decorator is non-interacitve object that just stand and
	// decorates map. Can serve as obstacles or be passable
	// All map decorators are stateless ( have only state notspawned and spawned )
	// If passable generates 
	class MapDecorator : public PhysicalObject
	{
	private:
		MapDecoratorDefinition* _mapDecoratorDef;

	public:
		MapDecorator(MapDecoratorDefinition* mdDef );
		~MapDecorator();

		MapDecoratorDefinition* GetMapDecoratorDefinition() const
		{ 
			return _mapDecoratorDef; 
		}

		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		void Spawn();
		void Despawn();
	};
}