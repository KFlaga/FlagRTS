#pragma once

#include "PathFindingUtils.h"
#include "PathFindingBoundingShapes.h"

namespace PathFinding
{
	template<typename VectorT = PFVector2>
	class IPathUnit
	{
	protected:
		CollisionFilter _filter;
		VectorT _position;
		VectorT _goal; // Position unit will try to reach ( used by local PF )
		VectorT _frameMove; // Position unit should move to in this frame
		float _maxSpeed; // Max speed ( in 1/ms )
		bool _requestPath; // If true path finder will try to move unit to '_goal', false if unit don't have goal

	public:
		IPathUnit(const CollisionFilter filter) :
			_filter(filter),
			_requestPath(false),
			_maxSpeed(0.f),
			_goal(0.f,0.f),
			_frameMove(0.f,0.f)
		{ }

		virtual void SetPosition(const VectorT& position) { _position = position; }
		const VectorT& GetPosition() const { return _position; }

		void SetGoal(const VectorT& goal) { _goal = goal; }
		const VectorT& GetGoal() const { return _goal; }
		
		void SetFrameMove(const VectorT& move) { _frameMove = move; }
		const VectorT& GetFrameMove() const { return _frameMove; }

		void SetMaxSpeed(const float maxMove) { _maxSpeed = maxMove; }
		float GetMaxSpeed() const { return _maxSpeed; }
		
		void SetCollsionFilter(const CollisionFilter filter) { _filter = filter; }
		CollisionFilter GetCollsionFilter() const { return _filter; }

		void SetIsPathRequested(bool value) { _requestPath = value; }
		bool GetIsPathRequested() const { return _requestPath; }

		bool BlocksUnit(const CollisionFilter otherUnitsFilter) const
		{
			return _filter.BlocksUnit(otherUnitsFilter);
		}

		bool BlocksUnit(const IPathUnit* otherUnit) const
		{
			return _filter.BlocksUnit(otherUnit->GetCollsionFilter());
		}
		
		bool IsBlockedBy(const CollisionFilter otherUnitsFilter) const
		{
			return _filter.IsBlockedBy(otherUnitsFilter);
		}

		bool IsBlockedBy(const IPathUnit* otherUnit) const
		{
			return _filter.IsBlockedBy(otherUnit->GetCollsionFilter());
		}

		virtual bool CollidesWith(IPathUnit<VectorT>* otherUnit) { return false; }

		virtual bool IsWithinSight(IPathUnit<VectorT>* otherUnit) 
		{ 
			return false;
		}

		virtual bool CollidesWith(const Box& otherUnitShape)
		{
			return false;
		}

		virtual bool CollidesWith(const Circle& otherUnitShape)
		{
			return false;
		}

		virtual bool IsWithinSight(const Box& otherUnitShape) 
		{ 
			return false;
		}

		virtual bool IsWithinSight(const Circle& otherUnitShape) 
		{ 
			return false;
		}

		virtual float GetDitanceTo(IPathUnit<VectorT>* otherUnit)
		{
			return (GetPosition() - otherUnit->GetPosition()).length();
		}

		virtual float GetDitanceTo(const Box& otherUnitShape)
		{
			return (GetPosition() - otherUnitShape.GetCenter()).length();
		}

		virtual float GetDitanceTo(const Circle& otherUnitShape)
		{
			return (GetPosition() - otherUnitShape.GetCenter()).length();
		}
	};

	// Generic path unit, to use in pathfinding/collisions etc, may use any shape for
	// BoundingShapeT / LookUpShapeT which exposes interface:
	// - Intersect() ( with both BoundingShapeT/LookUpShapeT and also one used in Dbvh )
	// - GetDistanceTo()
	// - Get/SetCenter()
	// - 
	// and uses VectorT as space position representation ( probably PFVector2/3 )
	// 
	template<typename BoundingShapeT = Box, typename LookUpShapeT = Box>
	class BoundedPathUnit2d : public IPathUnit<PFVector2>
	{
	public:
		typedef BoundedPathUnit2d<BoundingShapeT, LookUpShapeT> ThisT;
		typedef IPathUnit<PFVector2> BaseT;

	protected:
		BoundingShapeT _bounds; // Collision shape of unit
		LookUpShapeT _lookUpShape; // Shape unit will use as line of sight -> it will try to avoid units inside it
									 // look-up shape have same center/direction as unit
		PFVector2 _direction; // Current move direction of unit
		float _inertia; // Greater inertia will make unit more reluctant to change its optimal frame-goal. 
						// Range [0-1], if 1 unit will ignore other units when moving ( excluding collisions )


	public:
		BoundedPathUnit2d(const CollisionFilter filter) :
			IPathUnit(filter)
		{ }
		
		void SetPosition(const PFVector2& position) 
		{ 
			_position = position;
			_bounds.SetCenter(position); 
		}

		const BoundingShapeT& GetCollisionShape() const { return _bounds; }
		void SetCollisionShape(const BoundingShapeT& shape) { _bounds = shape; }

		const LookUpShapeT& GetLookUpShape() const { return _lookUpShape; }
		void SetLookUpShape(const LookUpShapeT& shape) { _lookUpShape = shape; }

		void SetDirection(const PFVector2& dir) { _direction = dir; }
		const PFVector2& GetDirection() const { return _direction; }
		
		float GetInertia() const { return _inertia; }
		void SetInertia(const float value) { _inertia = value; }

		// Try moving into target position ( move as close as possible )
		// and return bounding shape of moved unit
		virtual BoundingShapeT GetMovedShape(const PFVector2& target)
		{
			return _bounds;
		}

		bool CollidesWith(IPathUnit<PFVector2>* otherUnit) 
		{ 
			return otherUnit->CollidesWith(this->_bounds);
		}

		bool IsWithinSight(IPathUnit<PFVector2>* otherUnit) 
		{ 
			return otherUnit->CollidesWith(this->_lookUpShape);
		}

		bool IsWithinSight(const Box& otherUnitShape) 
		{ 
			return _lookUpShape.Intersect(otherUnitShape);;
		}

		bool IsWithinSight(const Circle& otherUnitShape) 
		{ 
			return _lookUpShape.Intersect(otherUnitShape);;
		}

		bool CollidesWith(const Box& otherUnitShape)
		{
			return _bounds.Intersect(otherUnitShape);
		}

		bool CollidesWith(const Circle& otherUnitShape)
		{
			return _bounds.Intersect(otherUnitShape);
		}

		virtual float GetDitanceTo(IPathUnit<PFVector2>* otherUnit)
		{
			return otherUnit->GetDitanceTo(this->_bounds);
		}

		float GetDitanceTo(const Box& otherUnitShape)
		{
			return _bounds.DistanceTo(otherUnitShape);
		}

		float GetDitanceTo(const Circle& otherUnitShape)
		{
			return _bounds.DistanceTo(otherUnitShape);
		}
	};
}