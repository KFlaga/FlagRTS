#include "PathingComponent.h"
#include "Unit.h"
#include "DefaultCommands.h"
#include "UnitStates.h"

#include "GameWorld.h"
#include "IPathingSystem.h"
#include "AnimationController.h"
#include "Map.h"

namespace FlagRTS
{
	PathingComponentDefinition::PathingComponentDefinition(XmlNode* compNode)
	{

	}

	PathingComponent::PathingComponent(ObjectDefinition* def, IGameObject* owner) :
		IObjectComponent(def, owner)
	{
		_ASSERT(dynamic_cast<PhysicalObject*>(_owner) != 0);
	}

	void PathingComponent::Update(float ms)
	{
		if(_frameMove.isZeroLength() == false)
		{
			GetOwner<SceneObject>()->Move(_frameMove);
			_frameMove = Vector3::ZERO;
		}

		Vector3 pos = GetOwner<SceneObject>()->GetPositionAbsolute();
		Vector2 pos2(pos.x, pos.z);
		if(_globalGoal.squaredDistance(pos2) < 100.f &&
			_velocity < 0.001f)
		{
			_targetObject = 0;
			_finishedMove = true;
			return;
		}

		if(_targetObject != 0)
		{
			Vector3 targetPos = _targetObject->GetPositionAbsolute();
			Vector2 targetPos2 = Vector2(targetPos.x, targetPos.z);

			// TODO: find some meaningful value to compare to
			if(targetPos2.squaredDistance(_globalGoal) > 2500.f)
			{
				_globalGoal = targetPos2;
				GameInterfaces::GetPathingSystem()->RequestPath(GetOwner<PhysicalObject>());
			}
		}
	}

	void PathingComponent::LoadResources(Ogre::SceneManager*)
	{

	}

	void PathingComponent::UnloadResources(Ogre::SceneManager*)
	{

	}

	void PathingComponent::OrderMoveToPoint(const Vector3& point)
	{
		_targetObject = 0;

		_globalGoal = Vector2(point.x, point.z);
		GameInterfaces::GetPathingSystem()->RequestPath(GetOwner<PhysicalObject>());
	}

	void PathingComponent::OrderMoveToObject(SceneObject* object)
	{
		_targetObject = object;
		Vector3 pos = object->GetPositionAbsolute();

		_globalGoal = Vector2(pos.x, pos.z);
		GameInterfaces::GetPathingSystem()->RequestPath(GetOwner<PhysicalObject>());
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
			PathingComponent* pathing = _executor->FindComponent<PathingComponent>();
			CollisionComponent* collision = _executor->FindComponent<CollisionComponent>();
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

				_pointTarget = _target.GetTargetUnit()->GetPositionAbsolute();
				if(collision->GetDistanceToOtherObject(_target.GetTargetUnit()) < _range)
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
		PathingComponent* pathing = _executor->FindComponent<PathingComponent>();
		_pointTarget = _target.GetPointTarget();
		_isFinished = false;

		// Get some better check -> object like bridge should be treated as
		// point target not object -> it could be resolved by query flags
		if(_target.GetTargetUnit() != 0)
		{
			_moveToObject = true;
			// If target vanishes during move, move to last target point
			// ( set move to object = false ) 
			_target.GetTargetUnit()->Despawned() += &_onTargetDespawned;

			pathing->OrderMoveToObject(_target.GetTargetUnit());
		}
		else
		{
			_moveToObject= false;
			pathing->OrderMoveToPoint(_target.GetPointTarget());
		}

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

	UnitMoveState::UnitMoveState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Move, "Move")
	{

	}

	void UnitMoveState::Begin()
	{
		_status = StateStatus::RunningNoncritical;

		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Move, "Default");

		PathingComponent* pathing = _owner->FindComponent<PathingComponent>();
		pathing->SetIsAllowDisplacement(true);
	}

	void UnitMoveState::End()
	{
		_status = StateStatus::Ready;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitMoveState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void UnitMoveState::Resume()
	{
		Begin();
	}

	void UnitMoveState::Update(float ms)
	{
		PathingComponent* pathing = _owner->FindComponent<PathingComponent>();

		if( pathing->IsFinishedMove() )
		{
			_status = StateStatus::Finished;
		}
		UnitState::Update(ms);
	}
}