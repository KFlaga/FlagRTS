#include "MapDecorator.h"
#include <OgreSceneManager.h>
#include "SceneObjectStates.h"
#include "MapDecoratorStates.h"

namespace FlagRTS
{
	MapDecorator::MapDecorator(MapDecoratorDefinition* def) :
		PhysicalObject(static_cast<PhysicalObjectDefinition*>(def)),
		_mapDecoratorDef(static_cast<MapDecoratorDefinition*>(_definition))
	{
		SetFinalType(GetTypeId<MapDecorator>());
	}

	MapDecorator::~MapDecorator()
	{

	}

	void MapDecorator::Spawn()
	{
		if(!_spawned)
		{
			PhysicalObject::Spawn();
			if( _stateMachine.FindState( SceneObjectStates::OnSpawn ) != 0)
				_stateMachine.ChangeState( SceneObjectStates::OnSpawn );
			else
				_stateMachine.ChangeState(SceneObjectStates::Idle);
			_spawned = true;
			_onSpawn(this);
		}
	}

	void MapDecorator::Despawn()
	{
		if(_spawned)
		{
			_stateMachine.ChangeState(SceneObjectStates::NotSpawned);
			PhysicalObject::Despawn();
			_spawned = false;
			_onDespawn(this);
		}
	}

	void MapDecorator::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		if(!_loaded)
		{
			PhysicalObject::LoadResources(ogreMgr);
			_loaded = true;
		}
	}

	void MapDecorator::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if(_loaded)
		{
			PhysicalObject::UnloadResources(ogreMgr);
			_loaded = false;
		}
	}
}