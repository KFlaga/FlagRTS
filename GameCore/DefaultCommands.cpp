#include "DefaultCommands.h"
#include "Unit.h"
#include "AnimationController.h"
#include "GameWorld.h"
#include "IObjectDefinitionManager.h"
#include "Map.h"
#include "Weapon.h"
#include "HoverTargetTypes.h"
#include "PathingSystem.h"
#include <PathFindingUniformGridPathingMap.h>
#include "UnitClass.h"
#include "ConstructionSiteData.h"
#include "TrainingQueue.h"
#include "Resources.h"
#include "DefaultCommandReqs.h"
#include "NoticeMessages.h"
#include "UnitStates.h"

#pragma warning(disable : 4244)

#include <Profiler.h>

namespace FlagRTS
{
	DoNothingCommand::DoNothingCommand(Unit* owner) :
		Command(owner, OrderTypes::NoCommand, 
		CommandCastTypes::Instant,
		CommandAligments::Neutral)
	{
		_appliance = CommandApplianceTypes::ToAll;
	}

	void DoNothingCommand::Update(float ms) { }
	void DoNothingCommand::Execute()
	{
		_isFinished = true;
	}

	Command* DoNothingCommand::GetCopy()
	{
		return xNew1(DoNothingCommand, _executor);
	}

	BasicMoveGroundCommand::BasicMoveGroundCommand(Unit* owner) :
		Command(owner, OrderTypes::Move, 
		CommandCastTypes::SingleTarget,
		CommandAligments::Neutral),
		_range(1.0f), 
		_onTargetDespawned(this)
	{
		_appliance = CommandApplianceTypes::ToAll;
		_requirements.push_back( xNew3(ExecutorIsConstructedRequirement,
			TechAvailability::Available, TechAvailability::Hidden, this));
		_requirements.push_back( xNew3(ExecutorIsImmobilizedRequirement,
			TechAvailability::Inavailable, TechAvailability::Available, this));
	}

	void BasicMoveGroundCommand::Update(float ms)
	{
		if(!_isFinished)
		{
			if(_moveToObject)
			{
				//	Vector2 oldTarget(_pointTarget.x,_pointTarget.z);

				//	// If target is object then get it most recent position
				//	_pointTarget = _target.GetTargetUnit()->GetPositionAbsolute();
				//	_pointTarget.y = GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(_pointTarget.x, _pointTarget.z);

				//	// Change unit final goal
				//	GameWorld::GlobalWorld->GetPathingSystem()->
				//		SetUnitGlobalGoal(_executor, _pointTarget);

				//	float cellSize = GameWorld::GlobalWorld->GetPathingSystem()->
				//		GetPathingMap()->GetCellSize().x;

				//	// If target moved 2 cells away from old position, recalculate path
				//	if(oldTarget.squaredDistance(Vector2(_pointTarget.x,_pointTarget.z)) > cellSize*cellSize*4)
				//	{
				//		GameWorld::GlobalWorld->GetPathingSystem()->RequestPathToGoal(_executor);
				//	}
				if(_executor->GetDistanceToOtherObject(_target.GetTargetUnit()) < _range)
				{
					Abort();
				}
			}
			else if(_executor->GetPositionAbsolute().squaredDistance(_pointTarget) < _range*_range)
			{
				Abort();
			}
		}
	}

	void BasicMoveGroundCommand::Execute()
	{
		// Get some better check -> object like bridge should be treated as
		// point target not object -> it could be resolved by query flags
		if(_target.GetTargetUnit() != 0)
		{
			// Treat as move to object
			_moveToObject = true;
			// If target vanishes during move, move to last target point
			// ( set move to object = false ) 
			_target.GetTargetUnit()->Despawned() += &_onTargetDespawned;
		}
		else
		{
			// Treat as move to point
			_moveToObject= false;
		}

		_pointTarget = _target.GetPointTarget();
		_pointTarget.y = GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(_pointTarget.x, _pointTarget.z);

		// Set target y = 0, units move only in x-z space
		_isFinished = false;

		// Set target for move and request path finding
		GameWorld::GlobalWorld->GetPathingSystem()->
			SetUnitGlobalGoal(_executor, _pointTarget, true, true);
	}

	inline void BasicMoveGroundCommand::Abort()
	{
		if(_moveToObject)
		{
			_target.GetTargetUnit()->Despawned() -= &_onTargetDespawned;
		}

		_isFinished = true;
		_range = 1.0f;
	}

	bool BasicMoveGroundCommand::IsTargetValid(CommandTarget* target) const
	{
		return (target->GetTargetType() & HoverTargetTypes::ObstacleGround) == 0;
	}

	Command* BasicMoveGroundCommand::GetCopy()
	{
		return xNew1(BasicMoveGroundCommand, _executor);
	}

	GroupMoveGroundCommand::GroupMoveGroundCommand(Unit* owner) :
		Command(owner, OrderTypes::MoveGroup, 
		CommandCastTypes::SingleTarget,
		CommandAligments::Neutral)
	{

	}

	void GroupMoveGroundCommand::Update(float ms)
	{

	}

	void GroupMoveGroundCommand::Execute()
	{

	}

	void GroupMoveGroundCommand::Abort()
	{

	}

	Command* GroupMoveGroundCommand::GetCopy()
	{
		return xNew1(GroupMoveGroundCommand, _executor);
	}

	bool GroupMoveGroundCommand::IsTargetValid(CommandTarget* target) const
	{
		return (target->GetTargetType() & HoverTargetTypes::ObstacleGround) == 0;
	}

	BasicAttackCommand::BasicAttackCommand(Unit* owner) :
		Command(owner, OrderTypes::Attack, 
		CommandCastTypes::SingleTarget,
		CommandAligments::Hostile),
		_onUnitDied(this)
	{
		_appliance = CommandApplianceTypes::ToAll;
		_requirements.push_back( xNew3(ExecutorIsConstructedRequirement,
			TechAvailability::Available, TechAvailability::Hidden, this));
		_requirements.push_back( xNew3(ExecutorIsDisarmedRequirement,
			TechAvailability::Inavailable, TechAvailability::Available, this));
	}

	void BasicAttackCommand::Update(float ms)
	{
		if( !_isFinished )
		{
			// In Update :
			// - checks if target moves out of range and if yes does same things as above
			// - if target died ends command
			Weapon* weapon = _executor->GetWeapons()->GetCurrentWeapon();
			if(_executor->GetDistanceToOtherObject(_unitTarget) > weapon->GetStats().Range)
			{
				// Unit out of range -> move closer to target
				BasicMoveGroundCommand* moveCommand = static_cast<BasicMoveGroundCommand*>(
					_executor->GetCommandQueue()->FindCommand(OrderTypes::Move, &_target));
				moveCommand->SetRange(weapon->GetStats().Range);
				moveCommand->SetTarget(_target);
				_executor->GetCommandQueue()->QueuePriorityCommand(moveCommand);
				return;
			}
			else if(_executor->GetDirectionAbsolute().angleBetween(
				_target.GetTargetUnit()->GetPositionAbsolute() - _executor->GetPositionAbsolute()).valueDegrees() >
				weapon->GetAttackAngle().valueDegrees() )
			{
				BasicRotateCommand* rotateCommand = static_cast<BasicRotateCommand*>(
					_executor->GetCommandQueue()->FindCommand(OrderTypes::Rotate, &_target));
				rotateCommand->SetTarget(_target);
				_executor->GetCommandQueue()->QueuePriorityCommand(rotateCommand);
				return;
			}
		}
	}

	void BasicAttackCommand::Execute()
	{
		_unitTarget = _target.GetTargetUnit();
		if( _unitTarget->IsAlive() == false )
		{
			_isFinished = true;
			return;
		}

		_isFinished = false;
		_unitTarget->UnitDied() += &_onUnitDied;

		// If target is within range :
		// - change state to attack,
		// If target is not within range :
		// - priority queue command move_to_range, attack command ahould execute just after m_t_r
		std::pair<Weapon*, bool> weapon = _executor->GetWeapons()->GetWeaponForTarget(&_target);
		if(weapon.first == 0)
		{
			// No weapon available for target -> should not happen actually
			// Just do nothing
			_isFinished = true;
			return;
		}

		if(weapon.second == false)
		{
			// Unit out of range -> move closer to target
			BasicMoveGroundCommand* moveCommand = static_cast<BasicMoveGroundCommand*>(
				_executor->GetCommandQueue()->FindCommand(OrderTypes::Move, &_target));
			if(moveCommand != 0 && 
				moveCommand->GetOrderType() == OrderTypes::Move &&
				_executor->IsImmobilized() == false)
			{
				// Unit can actually move
				moveCommand->SetRange(weapon.first->GetStats().Range);
				moveCommand->SetTarget(_target);
				_executor->GetCommandQueue()->QueuePriorityCommand(moveCommand);
			}
			else
			{
				// Unit cannot move, so abort command
				Abort();
			}
		}
		else 
		{
			// Got valid weapon, but still we need to be in proper direction
			// TODO some nicer check
			Vector3 dirExecutor = _executor->GetDirectionAbsolute();
			dirExecutor.y = 0.f;
			Vector3 dirTarget = _target.GetTargetUnit()->GetPositionAbsolute() - _executor->GetPositionAbsolute();
			dirTarget.y = 0.f;
			if(dirExecutor.angleBetween(dirTarget).valueDegrees() <	weapon.first->GetAttackAngle().valueDegrees() )
			{
				// Got valid weapon, so can start attack state
				_executor->GetWeapons()->SetCurrentWeapon(weapon.first);
				weapon.first->SetTarget(_target);
				_executor->ChangeState(SceneObjectStates::Attack);
			}
			else if(_executor->IsImmobilized() == false)
			{
				// We need to rotate towards target, so queue rotation command
				BasicRotateCommand* rotateCommand = static_cast<BasicRotateCommand*>(
					_executor->GetCommandQueue()->FindCommand(OrderTypes::Rotate, &_target));
				if(rotateCommand != 0 && rotateCommand->GetOrderType() == OrderTypes::Rotate )
				{
					// Unit can rotate, so do it
					rotateCommand->SetTarget(_target);
					_executor->GetCommandQueue()->QueuePriorityCommand(rotateCommand);
				}
				else
				{
					// Unit cannot rotate -> abort command
					Abort();
				}
			}
			else
			{
				// Unit cannot currently move/rotate
				Abort();
			}
		}
	}

	void BasicAttackCommand::OnUnitDied(Unit* target)
	{
		// Target died -> end command ( do not remove died handler, as it is called only once anyway )
		_isFinished = true;
		_executor->ChangeState(SceneObjectStates::Idle);
	}

	void BasicAttackCommand::Abort()
	{
		_isFinished = true;
		_unitTarget->UnitDied() -= &_onUnitDied;
		_executor->ChangeState(SceneObjectStates::Idle);
	}

	bool BasicAttackCommand::IsTargetValid(CommandTarget* target) const
	{
		return target->GetTargetUnit() != 0 &&
			UnitClasses::IsUnitOfSubClass(
			target->GetTargetUnit()->GetUnitClass(),
			UnitClasses::Ground);
	}

	Command* BasicAttackCommand::GetCopy()
	{
		return xNew1(BasicAttackCommand, _executor);
	}

	BasicRotateCommand::BasicRotateCommand(Unit* owner) :
		Command(owner, OrderTypes::Rotate, 
		CommandCastTypes::SingleTarget,
		CommandAligments::Neutral),
		_onTargetDespawned(this)
	{
		_appliance = CommandApplianceTypes::ToAll;
		_requirements.push_back( xNew3(ExecutorIsConstructedRequirement,
			TechAvailability::Available, TechAvailability::Hidden, this));
		_requirements.push_back( xNew3(ExecutorIsImmobilizedRequirement,
			TechAvailability::Inavailable, TechAvailability::Available, this));
	}

	void BasicRotateCommand::Update(float ms)
	{
		if(!_isFinished)
		{
			if(_rotateTowardsObejct)
			{
				// Update target direction as it might change 
				UnitForceMoveState* moveState = static_cast<UnitForceMoveState*>(
					_executor->FindState(SceneObjectStates::ForceMove));
				moveState->SetTarget(_target.GetPointTarget());
			}

			if(_rotateStrategy.GetRemainingAngle() <= 1e-4f)
			{
				Abort();
			}
		}
	}

	void BasicRotateCommand::Execute()
	{
		if(_executor->IsImmobilized())
		{
			// Unit cannot currently move, so do nothing
			Abort();
			return;
		}

		// Get some better check -> object like bridge should be treated as
		// point target not object -> it could be resolved by query flags
		if(_target.GetTargetUnit() != 0)
		{
			// Treat as move to object
			_rotateTowardsObejct = true;
			// If target vanishes during move, move to last target point
			// ( set move to object = false ) 
			_target.GetTargetUnit()->Despawned() += &_onTargetDespawned;
		}
		else
		{
			// Treat as move to point
			_rotateTowardsObejct = false;
		}

		_executorStrategy = _executor->GetMoveStrategy();
		_executor->SetMoveStrategy(&_rotateStrategy);
		_rotateStrategy.ResetRemainingDistance();

		// Force movement using RotateOnlyStrategy ( when unit have rotate command, it 
		// should have UnitForceMoveState or its subclass )
		UnitForceMoveState* moveState = static_cast<UnitForceMoveState*>(
			_executor->FindState(SceneObjectStates::ForceMove));
		moveState->SetTarget(_target.GetPointTarget());
		_executor->ChangeState(SceneObjectStates::ForceMove);

		_isFinished = false;
	}

	void BasicRotateCommand::Abort()
	{
		if(_rotateTowardsObejct)
		{
			_target.GetTargetUnit()->Despawned() -= &_onTargetDespawned;
		}

		// Reset rotation goal and mover
		_executor->SetMoveStrategy(_executorStrategy);
		_executor->MoveUnit(_executor->GetPositionAbsolute(), 0.f); // Reset distance, so unit can exit forced movement if it was set

		_isFinished = true;
	}

	bool BasicRotateCommand::IsTargetValid(CommandTarget* target) const
	{
		return true;
	}

	Command* BasicRotateCommand::GetCopy()
	{
		return xNew1(BasicRotateCommand, _executor);
	}

	AbortBuildingConstructionCommand::AbortBuildingConstructionCommand(Unit* owner) :
		Command(owner, OrderTypes::Stop, 
		CommandCastTypes::Instant,
		CommandAligments::Neutral)
	{
		_appliance = CommandApplianceTypes::ToSingle;
		_requirements.push_back( xNew3(ExecutorIsConstructedRequirement,
			TechAvailability::Hidden, TechAvailability::Available, this));
	}

	void AbortBuildingConstructionCommand::Update(float ms)
	{

	}

	void AbortBuildingConstructionCommand::Execute()
	{
		// _executor->KillUnit();
		// return resources
		_isFinished = true;
	}

	void AbortBuildingConstructionCommand::Abort()
	{
		_isFinished = true;
	}

	Command* AbortBuildingConstructionCommand::GetCopy()
	{
		return xNew1(AbortBuildingConstructionCommand, _executor);
	}

	bool AbortBuildingConstructionCommand::IsTargetValid(CommandTarget* target) const
	{
		_ASSERT(target->GetTargetUnit() != 0);
		return UnitClasses::IsUnitOfSubClass(
			target->GetTargetUnit()->GetUnitClass(),
			UnitClasses::Structure) &&
			target->GetTargetUnit()->IsUnderConstruction();
	}

	TrainUnitCommand::TrainUnitCommand(Unit* owner, UnitDefinition* trainedUnit, int trainNumber) :
		Command(owner, (OrderType)(OrderTypes::Train0 + trainNumber), 
		CommandCastTypes::Instant,
		CommandAligments::Neutral),
		_trainedUnit(trainedUnit)
	{
		_appliance = CommandApplianceTypes::ToSingle;

		_requirements.push_back( xNew3(ExecutorIsConstructedRequirement,
			TechAvailability::Available, TechAvailability::Hidden, this));
		//	_requirements.push_back( xNew3(ExecutorIsSiliencedRequirement,
		//		TechAvailability::Inavailable, TechAvailability::Available, this));
	}

	TrainUnitCommand::TrainUnitCommand(Unit* owner, const char* trainedUnitName, int trainNumber) :
		Command(owner, (OrderType)(OrderTypes::Train0 + trainNumber), 
		CommandCastTypes::Instant,
		CommandAligments::Neutral),
		_trainedUnit(0)
	{
		_appliance = CommandApplianceTypes::ToSingle;
		_trainedUnit = static_cast<UnitDefinition*>(GameInterfaces::GetObjectDefinitionManager()->
			GetObjectDefinitionByName("Unit", trainedUnitName));

		_requirements.push_back( xNew3(ExecutorIsConstructedRequirement,
			TechAvailability::Available, TechAvailability::Hidden, this));
		//	_requirements.push_back( xNew3(ExecutorIsSiliencedRequirement,
		//		TechAvailability::Inavailable, TechAvailability::Available, this));
	}

	void TrainUnitCommand::Update(float ms)
	{
		// Do nothing
	}

	void TrainUnitCommand::Execute()
	{
		_isFinished = true;
		_executor->GetTrainingQueue()->AddToQueue(_trainedUnit);
	}

	void TrainUnitCommand::Abort()
	{
		_isFinished = true;
	}

	Command* TrainUnitCommand::GetCopy()
	{
		return xNew3(TrainUnitCommand, _executor, _trainedUnit, GetOrderType() - OrderTypes::Train0);
	}

	bool TrainUnitCommand::IsTargetValid(CommandTarget* target) const
	{
		return true; // Target-less command
	}
}