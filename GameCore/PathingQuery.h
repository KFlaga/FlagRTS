#pragma once

#include "IPathingQuery.h"
#include <PathFindingDbvhQueries.h>

namespace FlagRTS
{
	class BoxDbvhBoxObjectPathingQuery : public IBoxPathingQuery, 
		public PathFinding::DbvhQuery<PathFinding::Box, PathFinding::Box, PhysicalObject>
	{
		typedef PathFinding::DbvhQuery<PathFinding::Box, PathFinding::Box, PhysicalObject> DbvhQuery;
		PathFinding::Dbvh<PathFinding::Box>* _dbvh;

	public:
		BoxDbvhBoxObjectPathingQuery(PathFinding::Dbvh<PathFinding::Box>* dbvh) :
			_dbvh(dbvh)
		{ }

		void SetTestShape(const PathFinding::Box& shape)
		{
			DbvhQuery::SetTestShape(shape);
		}

		void Execute()
		{
			DbvhQuery::Execute(_dbvh);
		}

		void Execute(Dyscriminator& acceptHit)
		{
			DbvhQuery::Execute(_dbvh, acceptHit);
		}

		template<typename DyscriminatorT>
		void Execute(DyscriminatorT& acceptHit)
		{
			DbvhQuery::Execute(_dbvh, acceptHit);
		}

		PFArray<PhysicalObject*>& GetObjectsHit()
		{
			return DbvhQuery::GetObjectsHit();
		}
	};
}