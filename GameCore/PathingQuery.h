#pragma once

#include "IPathingQuery.h"
#include "BoundingShapes.h"
#include "PathingDbvh.h"
#include "CollisionShapes.h"
#include "CollisionComponent.h"
#include "PhysicalObject.h"

namespace FlagRTS
{
	struct AcceptAllDyscryminator
	{
		inline bool operator()(PhysicalObject* object)
		{
			return true;
		}
	};
	
	struct BlockedByTestObjectDyscryminator
	{
		CollisionFilter _filter;
		BlockedByTestObjectDyscryminator(CollisionFilter filter)
		{
			_filter = filter;
		}

		inline bool operator()(PhysicalObject* object)
		{
			_filter.Blocks(object->FindComponent<CollisionComponent>()->GetCollisionFilter());
			return true;
		}
	};
	
	struct BlocksTestObjectDyscryminator
	{
		CollisionFilter _filter;
		BlocksTestObjectDyscryminator(CollisionFilter filter)
		{
			_filter = filter;
		}

		inline bool operator()(PhysicalObject* object)
		{
			_filter.IsBlockedBy(object->FindComponent<CollisionComponent>()->GetCollisionFilter());
			return true;
		}
	};

	class BoxDbvhQuery : public IPathingQuery
	{
	protected:
		Dbvh<BoundingBox>* _dbvh;

	public:
		BoxDbvhQuery(Dbvh<BoundingBox>* dbvh) { }

		bool Intersect(const BoundingBox& dbvhShape)
		{
			return _collisionShape->GetBoundingBox().Intersect(dbvhShape);
		}

		void Execute()
		{
			Execute_Custom(AcceptAllDyscryminator());
		}
		
		void Execute(Dyscriminator& d)
		{
			Execute_Custom(d);
		}
		
		void Execute_CatchBlockedByTestObject()
		{
			Execute_Custom(BlockedByTestObjectDyscryminator(_collisionShape->GetCollisionFilter()));
		}
		
		void Execute_CatchBlocksTestObject()
		{
			Execute_Custom(BlocksTestObjectDyscryminator(_collisionShape->GetCollisionFilter()));
		}
		
		template<typename DysryminatorType = AcceptAllDyscryminator>
		void Execute_Custom(DysryminatorType& acceptHit = DysryminatorType())
		{
			_hitObjects.clear();
			_dbvh->TestGeneric(_collisionShape->GetBoundingBox(), _hitObjects, acceptHit);
		}
	};
}