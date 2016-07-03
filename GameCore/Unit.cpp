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
		_weapons(this, def->GetWeapons()),
		_currentStats(def->GetStats()),
		_finalUnitClass(0),
		_isInteractable(true),
		_aiEnabled(true),
		_moveStrategy(def->GetMoveStrategy()),
		_isUnderConstruction(false),
		_constructionProgress(1.f),
		_trainQueue(0),
		_rallyPoint(-1.f,-1.f,-1.f),
		_isImmobilized(false),
		_isSilienced(false),
		_isDisarmed(false),
		_isSelected(false),
		_selectionMarker(0),
		_isAlive(true)
	{
		SetFinalType(GetTypeId<Unit>());
		_onConstructionFinsished = def->GetOnConstructionFinishedEvent();
		if(CanTrainUnits())
		{
			_trainQueue = xNew1(TrainingQueue, this);
		}

		_onIsSelectedChanged = def->GetOnIsSelectedChangedEvent();
	}

	Unit::~Unit()
	{
		xDeleteSafe(_trainQueue);
	}

	void Unit::Spawn()
	{
		if(!_spawned)
		{
			PhysicalObject::Spawn();

			if( _selectionMarker != 0 )
			{
				// Create node for marker, attach it (it will create child node) + set not visible
				AttachChild(_selectionMarker, true);
				_selectionMarker->Spawn();
				_selectionMarker->GetOgreMovable()->setVisible(false);
				// Set initial values for marker
				_selectionMarker->SetSize(_unitDef->GetSelectionMarkerSize());
				_selectionMarker->SetPosition(_unitDef->GetSelectionMarkerOffset());
			}

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

			if( _selectionMarker != 0 )
			{
				_selectionMarker->Despawn();
				DetachChild(_selectionMarker, true, true);
			}
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
			if( _unitDef->GetSelectionMarkerDefinition() != 0 )
			{
				// Create selection marker
				_selectionMarker = static_cast<SceneMarker*>(
					GameWorld::GlobalWorld->CreateSceneObject(
					_unitDef->GetSelectionMarkerDefinition(), NEUTRAL_PLAYERNUM));
			}
			_loaded = true;
		}
	}

	void Unit::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if(_loaded)
		{
			if( _selectionMarker != 0)
			{
				GameWorld::GlobalWorld->DestroySceneObject(_selectionMarker);
				_selectionMarker = 0;
			}
			PhysicalObject::UnloadResources(ogreMgr);
			_loaded = false;
		}
	}

	void Unit::Update(float ms)
	{
		_weapons.Update(ms);
		PhysicalObject::Update(ms);
	}

	void Unit::SetOwner(unsigned int owner)
	{
		RemoveSelectionFlags(SelectionFlags::GetPlayerFlag(_owner));
		PhysicalObject::SetOwner(owner);
		if(_selectionMarker != 0)
			_selectionMarker->SetOwner(owner);
		AddSelectionFlags(SelectionFlags::GetPlayerFlag(owner));
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
				GameWorld::GlobalWorld->QueueDespawnSceneObject(this);
				GameWorld::GlobalWorld->QueueDestroySceneObject(this);
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
		IMoveStrategy* move = static_cast<IMoveStrategy*>(GameWorld::GlobalWorld->
			GetGameObjectDefinition("MoveStrategy", name));
		if( move != 0 )
			_moveStrategy = move;
	}

	bool Unit::TryFinishTrain(Unit* trainedUnit)
	{
		UnitTrainingState* curState = dynamic_cast<UnitTrainingState*>(_stateMachine.GetCurrentStatePtr());
		if( curState != nullptr )
		{
			return curState->TryFinishTrain(trainedUnit);
		}
		return false;
	}

	void Unit::SetIsSelected(bool value)
	{
		if( value != _isSelected )
		{
			_isSelected = (value);
			if(_selectionMarker != 0)
			{
				_selectionMarker->GetOgreMovable()->setVisible(_isSelected);
			}
			_onIsSelectedChanged.Fire(this, value);
		}
	}

	bool Unit::IsTrainingUnit() const
	{
		if( _trainQueue != 0 )
			return _trainQueue->IsTraining();
		else
			return false;
	}

	void Unit::SetUnitClass(UnitClass uclass) 
	{ 
		_finalUnitClass = uclass;
		uint32 newFlags = _selectionFlags;
		UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::SubClasses::Structure) ?
			newFlags | SelectionFlags::Building : newFlags & (~SelectionFlags::Building);

		UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::SubClasses::Worker) ||
			UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::SubClasses::TransportResources)	?
			newFlags | SelectionFlags::Worker : newFlags & (~SelectionFlags::Worker);

		UnitClasses::IsUnitOfSubClass(uclass, UnitClasses::SubClasses::Resource) ?
			newFlags | SelectionFlags::Resource : newFlags & (~SelectionFlags::Resource);

		newFlags == _selectionFlags ?
			newFlags | SelectionFlags::Military :  newFlags & (~SelectionFlags::Military);

		SetSelectionFlags(newFlags);
	}
}