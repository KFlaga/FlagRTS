#pragma once

#include "IObjectComponent.h"
#include "ObjectDefinition.h"
#include "CollisionComponent.h"
#include <IntVector2.h>

namespace FlagRTS
{
	class SceneObject;
	class PhysicalObject;
	class Unit;
	class CollisionComponent;
	class IMoveStrategy;

	/**
	Expected layout of node:
	<Component type="MinimapComponent">
	<Flags>
	<RevealedVisible/>
	<UseCustomIcon/>
	<MinimapSelectable/>
	</Flags>
	<Size x="80.f" y="160.f"/> <!-- Size of minimap icon -> whole minimap is 1000x1000 * aspect -->
	<CustomIcon name="icon.png"/> <!-- Must be provided if flag UseCustomIcon is set -->
	</Component>
	*/
	class PathingComponentDefinition : public ObjectDefinition
	{
	protected:
		IMoveStrategy* _defualtMoveStrategy;

		// Movement stats (for use by IMoveStartegy)
		float _maxVelocity;
		float _acceleration;
		float _turnRate;

	public:
		PathingComponentDefinition(XmlNode* compNode);

		float GetMaxVelocity() const { return _maxVelocity; }
		void SetMaxVelocity(float val) { _maxVelocity = val; }
		
		float GetAcceleration() const { return _maxVelocity; }
		void SetAcceleration(float val) { _maxVelocity = val; }
		
		float GetTurnRate() const { return _maxVelocity; }
		void SetTurnRate(float val) { _maxVelocity = val; }
	};

	/**
	Allows unit to use pathfinding to request path and move along it.

	Exposes interface of PathFiniding::IPathUnit so ot may be used with PathFiniding

	Owner must be a Unit.
	To use PathingComponent unit must have also a CollisionComponent.
	*/
	class PathingComponent : public IObjectComponent
	{
	protected:
		CollisionComponent* _collisionComponent;
		IMoveStrategy* _moveStrategy;

		// GlobalPath
		Vector2 _globalGoal;
		IntVector2 _sizeInCells;
		Array<Vector2> _globalPath;
		int _pathIndex;

		// LocalPath
		Vector3 _frameMove;
		Vector2 _localGoal;

		// Movement
		SceneObject* _targetObject;
		float _velocity;

		// Flags
		bool _requestedGlobalPath;
		bool _requestedLocalPath;
		bool _finishedMove;
		bool _allowDisplacement;

	public:
		PathingComponent(ObjectDefinition* def, IGameObject* owner);

		void Update(float ms);
		void LoadResources(Ogre::SceneManager*);
		void UnloadResources(Ogre::SceneManager*);

		const CollisionShape& GetCollisionShape() const
		{
			return _collisionComponent->GetCollisionShape();
		}
		
		CollisionShape& GetCollisionShape()
		{
			return _collisionComponent->GetCollisionShape();
		}

		void OrderMoveToPoint(const Vector3& point);
		void OrderMoveToObject(SceneObject* object);
		
		float GetMaxVelocity() const { return GetDefinition<PathingComponentDefinition>()->GetMaxVelocity(); }	
		float GetAcceleration() const { return GetDefinition<PathingComponentDefinition>()->GetAcceleration(); }
		float GetTurnRate() const { return GetDefinition<PathingComponentDefinition>()->GetTurnRate(); }
		float GetVelocity() const { return _velocity; }
		void SetVelocity(float val) { _velocity = val; }

	public: // PathinSystem Unit interface
		bool IsRequestedGlobalPath() const { return _requestedGlobalPath; }
		void SetIsRequestedGlobalPath(bool val) { _requestedGlobalPath = val; }
		
		bool IsFinishedMove() const { return _finishedMove; }
		void SetIsFinishedMove(bool val) { _finishedMove = val; }

		bool IsRequestedLocalPath() const { return _requestedLocalPath; }
		void SetIsRequestedLocalPath(bool val) { _requestedLocalPath = val; }
		
		bool IsAllowDisplacement() const { return _allowDisplacement; }
		void SetIsAllowDisplacement(bool val) { _allowDisplacement = val; }

		Array<Vector2>& GetGlobalPath() { return _globalPath; }

		int GetCurrentPathIndex() const { return _pathIndex; }
		void SetCurrentPathIndex(int idx) {	_pathIndex = idx; }

		IntVector2 GetSizeInCells() const { return _sizeInCells; }
		void SetSizeInCells(IntVector2 size) { _sizeInCells = size; }

	public: // PathFiniding::IPathUnit interface
		Vector2 GetPosition() const
		{
			return Vector2(
				GetCollisionShape().GetCenter().x,
				GetCollisionShape().GetCenter().z);
		}

		void SetGlobalGoal(const Vector2& goal) { _globalGoal = goal; }
		Vector2 GetGlobalGoal() const { return _globalGoal; }
		
		void SetLocalGoal(const Vector2& goal) { _localGoal = goal; }
		Vector2 GetLocalGoal() const { return _localGoal; }
		
		void SetFrameMove(const Vector3& move) { _frameMove = move; }
		Vector3 GetFrameMove() const { return _frameMove; }

		void SetCollsionFilter(const CollisionFilter filter)
		{ 
			GetCollisionShape().SetCollisionFilter(filter);
		}

		CollisionFilter GetCollsionFilter() const 
		{ 
			return GetCollisionShape().GetCollisionFilter(); 
		}

		bool BlocksUnit(PathingComponent* otherUnit) const
		{
			return GetCollsionFilter().Blocks(otherUnit->GetCollsionFilter());
		}

		bool IsBlockedBy(PathingComponent* otherUnit) const
		{
			return GetCollsionFilter().IsBlockedBy(otherUnit->GetCollsionFilter());
		}

		bool CollidesWith(PathingComponent* otherUnit)  const
		{ 
			return GetCollisionShape().CheckCollidesWith(
				otherUnit->GetCollisionShape());
		}

		bool IsWithinSight(PathingComponent* otherUnit) const
		{ 
			return false;
		}

		// bool IsOnCollisionPath(PathingComponent* otherUnit) const;

		float GetDitanceTo(PathingComponent* otherUnit)
		{
			return GetCollisionShape().DistanceTo(otherUnit->GetCollisionShape());
		}
	};
}