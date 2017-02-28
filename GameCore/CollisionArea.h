#pragma once

#include "CollisionShapes.h"
#include <Array.h>
#include "IPathingQuery.h"
#include "DataTypes.h"

namespace FlagRTS
{
	class Unit;
	class CheckCollision_AreaBox;
	class CheckCollision_AreaCircle;
	class PhysicalObject;

	// Simple class allowing for collision check with units in area
	// ( for missles, planners etc. )
	// Works with all CollisionShapeType except footprint / hull
	// For box/circle only (x,z) parts of vectors are used
	// For sphere/circle radius is in extends.x
	// For cone extends.x is base radius, extends.y is height
	class CollisionArea
	{
	protected:
		PhysicalObject* _owner; // Optional owner -> CA will not register collisions with owner
		IBoxPathingQuery* _query;
		CollisionShapeType _shape;
		Vector3 _center;
		Vector3 _halfExtends;
		//Quaternion _orientation;
		int _filter;

	public:
		CollisionArea(
			int collisionFilter,
			CollisionShapeType shape, 
			const Vector3& center, 
			const Vector3& halfExtends);
		~CollisionArea();

		void SetCenter(const Vector3& vec) { _center = vec; }
		void SetHalfExtends(const Vector3& vec) { _halfExtends = vec; }
		void SetShapeType(CollisionShapeType shape) { _shape = shape; } 
		void SetCollisionFilter(int filter) { _filter = filter; }
		void SetOwner(PhysicalObject* owner) { _owner = owner; }
		
		// Finds collision using PathingSystem, only registered PathUnits are hit
		// which collides with given filter ( this filter must block unit pathing group )
		void FindCollisions();

		// Returns true if areas collide
		bool CheckCollideWith(CollisionArea* otherArea);

		Array<Unit*>& GetUnitsHit() 
		{
			return reinterpret_cast<Array<Unit*>&>(_query->GetObjectsHit());
		}

		friend CheckCollision_AreaBox;
		friend CheckCollision_AreaCircle;
	};
}