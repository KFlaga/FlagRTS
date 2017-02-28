#include "Unit.h"
#include <OgreSceneManager.h>
#include "SceneObjectStates.h"
#include "UnitStates.h"
#include "SelectionFlags.h"
#include "Weapon.h"

#include <OgreSubEntity.h>
#include "DefaultMoveStrategies.h"
#include "TrainingQueue.h"
#include "SceneMarker.h"
#include "GameWorld.h"
#include "ISceneObjectSpawner.h"
#include "IGameObjectPool.h"
#include "IObjectDefinitionManager.h"
#include "Player.h"
#include "UnitClass.h"
#include "PathingSystem.h"
#include "IMoveStrategy.h"

namespace FlagRTS
{
	Unit::Unit(UnitDefinition* def) :
		PhysicalObject(static_cast<PhysicalObjectDefinition*>(def)),
		_unitDef(static_cast<UnitDefinition*>(_definition)),
		_commands(this),
		_currentStats(def->GetStats()),
		_finalUnitClass(0),
		_isInteractable(true),
		_aiEnabled(true),
		_moveStrategy(def->GetMoveStrategy()),
		_isUnderConstruction(false),
		_constructionProgress(1.f),
		_rallyPoint(-1.f,-1.f,-1.f),
		_isImmobilized(false),
		_isSilienced(false),
		_isDisarmed(false),
		_isAlive(true)
	{
		SetFinalType(GetTypeId<Unit>());
		_onConstructionFinsished = def->GetOnConstructionFinishedEvent();
		
	}

	Unit::~Unit()
	{

	}

	void Unit::Spawn()
	{
		if(!_spawned)
		{
			PhysicalObject::Spawn();

			if( _stateMachine.FindState( SceneObjectStates::OnSpawn ) != 0)
				_stateMachine.ChangeState( SceneObjectStates::OnSpawn );
			else
				_stateMachine.ChangeState(SceneObjectStates::Idle);
			_spawned = true;
			_isAlive = true;
			_onSpawn(this);
			_onSpawn.Clear();
		}
	}

	void Unit::Despawn()
	{
		if(_spawned)
		{
			if( _stateMachine.FindState( SceneObjectStates::OnDespawn ) != 0)
				_stateMachine.ChangeState( SceneObjectStates::OnDespawn );
			else
				_stateMachine.ChangeState(SceneObjectStates::NotSpawned);

			PhysicalObject::Despawn();
			_spawned = false;
			_onDespawn(this);
			_onDespawn.Clear();
		}
	}

	void Unit::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		if(!_loaded)
		{
			PhysicalObject::LoadResources(ogreMgr);
			_loaded = true;
		}
	}

	void Unit::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if(_loaded)
		{
			PhysicalObject::UnloadResources(ogreMgr);
			_loaded = false;
		}
	}

	void Unit::Update(float ms)
	{
		PhysicalObject::Update(ms);
	}

	// TODO add weapon source to determine if it was friendly fire or maybe some special death effect 
	// (burning agony). Then somehow make it work for abilities
	void Unit::InflictDamage(int damage, Weapon* source) 
	{
		_currentStats.HitPoints -= damage;
		if(_currentStats.HitPoints <= 0)
		{
			// Unit died
			KillUnit(0, false);
		}
	}

	void Unit::KillUnit(int reason, bool instant)
	{
		if( IsAlive() ) // Make sure we dont kill dead unit
		{
			_isAlive = false;
			if(instant)
			{
				// Despawn object on next frame
				GameInterfaces::GetSceneObjectSpawner()->DespawnSceneObject(this, true);
				GameInterfaces::GetSceneObjectSpawner()->DestroySceneObject(this, true);
			}
			else
			{
				// Enter dying state
				ChangeState(SceneObjectStates::Dying);
			}
			_onDied.Fire(this); 
			_onDied.Clear();
		}
	}

	void Unit::SetMoveStrategy(const string& name)
	{
		IMoveStrategy* move = static_cast<IMoveStrategy*>(GameInterfaces::GetObjectDefinitionManager()->
			GetObjectDefinitionByName("MoveStrategy", name));
		if( move != 0 )
			_moveStrategy = move;
	}

	void Unit::SetUnitClass(UnitClass uclass) 
	{ 
		_finalUnitClass = uclass;
		/*uint32 newFlags = _selectionFlags;
		UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::SubClasses::Structure) ?
			newFlags | SelectionFlags::Building : newFlags & (~SelectionFlags::Building);

		UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::SubClasses::Worker) ||
			UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::SubClasses::TransportResources)	?
			newFlags | SelectionFlags::Worker : newFlags & (~SelectionFlags::Worker);

		UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::SubClasses::Resource) ?
			newFlags | SelectionFlags::Resource : newFlags & (~SelectionFlags::Resource);

		newFlags == _selectionFlags ?
			newFlags | SelectionFlags::Military :  newFlags & (~SelectionFlags::Military);

		SetSelectionFlags(newFlags);*/
	}
}