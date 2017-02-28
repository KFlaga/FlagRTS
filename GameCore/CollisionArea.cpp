#include "CollisionArea.h"
#include "PathingSystem.h"
#include "GameWorld.h"
#include "Unit.h"
#include <PathFindingUtils.h>

namespace FlagRTS
{
	class CheckCollision_AreaBox : public Dyscriminator
	{
	public:
		CheckCollision_AreaBox(CollisionArea* area) :
			TestArea(area)
		{ }

		CollisionArea* TestArea;
		bool operator()(PhysicalObject* object)
		{
			if( object == TestArea->_owner ||
				object->GetFinalType() != GetTypeId<Unit>() ||
				!(PathFinding::CollisionFilter(TestArea->_filter).IsBlockedBy(PathFinding::CollisionFilter(
				object->GetPathingGroup(),object->GetPathingBlockedGroups()))) )
				return false;

			// Here should just return true, but later test agains arbitrary unit's collision area will be added

			auto aabb = object->GetBoudningBox();
			return !(TestArea->_center.x - TestArea->_halfExtends.x > aabb.getMaximum().x ||
				TestArea->_center.x + TestArea->_halfExtends.x < aabb.getMinimum().x ||
				TestArea->_center.z + TestArea->_halfExtends.z < aabb.getMinimum().z ||
				TestArea->_center.z + TestArea->_halfExtends.z < aabb.getMinimum().z );
		}
	};

	class CheckCollision_AreaCircle : public Dyscriminator
	{
	public:
		CheckCollision_AreaCircle(CollisionArea* area) :
			TestArea(area)
		{ }

		CollisionArea* TestArea;
		bool operator()(PhysicalObject* object)
		{
			if( object == TestArea->_owner ||
				object->GetFinalType() != GetTypeId<Unit>() ||
				!(PathFinding::CollisionFilter(TestArea->_filter).BlocksUnit(PathFinding::CollisionFilter(
				object->GetPathingGroup(),object->GetPathingBlockedGroups()))) )
				return false;

			auto aabb = object->GetBoudningBox();
			if( (TestArea->_center.x - TestArea->_halfExtends.x > aabb.getMaximum().x ||
				TestArea->_center.x + TestArea->_halfExtends.x < aabb.getMinimum().x ||
				TestArea->_center.z + TestArea->_halfExtends.z < aabb.getMinimum().z ||
				TestArea->_center.z + TestArea->_halfExtends.z < aabb.getMinimum().z ) )
				return false;

			// Need to check corners only
			float radius2 = TestArea->_halfExtends.x * TestArea->_halfExtends.x;
			Vector2 center = Vector2(TestArea->_halfExtends.x, TestArea->_halfExtends.z);
			if( center.squaredDistance(Vector2(aabb.getMaximum().x, aabb.getMaximum().z)) <= radius2 ||
				center.squaredDistance(Vector2(aabb.getMinimum().x, aabb.getMaximum().z)) <= radius2 ||
				center.squaredDistance(Vector2(aabb.getMaximum().x, aabb.getMinimum().z)) <= radius2 ||
				center.squaredDistance(Vector2(aabb.getMinimum().x, aabb.getMinimum().z)) <= radius2 )
				return true;

			// BBoxes intersects, but circle dont
			return false;
		}
	};

	CollisionArea::CollisionArea(
		int collisionFilter,
		CollisionShapeType shape, 
		const Vector3& center, 
		const Vector3& halfExtends) :
	_filter(collisionFilter),
		_shape(shape),
		_center(center),
		_halfExtends(halfExtends),
		_owner(0)
	{
		_query = GameWorld::GlobalWorld->GetPathingSystem()->CreateBoxPathingQuery();
	}

	CollisionArea::~CollisionArea()
	{
		GameWorld::GlobalWorld->GetPathingSystem()->DestroyBoxPathingQuery(_query);
	}

	void CollisionArea::FindCollisions()
	{
		_query->SetTestShape( BoundingBox( 
			Vector2(_center.x - _halfExtends.x, _center.z - _halfExtends.z),
			Vector2(_center.x + _halfExtends.x, _center.z + _halfExtends.z)) );

		switch(_shape)
		{
		case CollisionShapes::Box:
			{
				CheckCollision_AreaBox collCheckAB(this);
				_query->Execute(collCheckAB);
			}
			break;
		case CollisionShapes::Circle:
			{
				CheckCollision_AreaCircle collCheckAC(this);
				_query->Execute(collCheckAC);
			}
			break;
		}
	}

	bool CollisionArea::CheckCollideWith(CollisionArea* otherArea)
	{
		return false;
	}

	CollisionShape::CollisionShape()
	{

	}

	CollisionShape::CollisionShape(
		CollisionFilter collisionFilter,
		CollisionShapeType shape, 
		const Vector3& center, 
		const Vector3& halfExtends) :
	    _collisionFilter(collisionFilter),
		_shapeType(shape),
		_center(center),
		_halfExtends(halfExtends)
	{
	
	}
}