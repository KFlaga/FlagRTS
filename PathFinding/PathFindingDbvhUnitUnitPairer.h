#pragma once

#include "PathFindingIPathUnit.h"
#include "PathFindingDbvh.h"
#include "PathFindingDbvhQueries.h"
#include "PathFindingIUnitUnitPairer.h"

namespace PathFinding
{
	// Most generic query to find units that are within other's look-up shape
	// Assumes that Dbvh may work with different shapes than LookupShapeT and UnitBoundsShapeT, 
	// otherwise checks are duplicated
	// Needs GetPathUnitFuncT functor which converts ObjT* to BoundedPathUnit<UnitBoundsShapeT,LookupShapeT,VectorT>*
	template<typename DbvhShapeT, typename ObjT, typename GetPathUnitFuncT>
	class FindUnitsWithinLookupShapeQuery2d
	{
	public:
		typedef IPathUnit<PFVector2> UnitT;

	protected:
		PFArray<ObjT*> _hitObjects;
		UnitT* _testedUnit;
		GetPathUnitFuncT _getPathUnit;

	public:
		FindUnitsWithinLookupShapeQuery2d(GetPathUnitFuncT getPathUnit = GetPathUnitFuncT()) :
			_getPathUnit(getPathUnit)
		{ }

		void SetTestedUnit(UnitT* unit) { _testedUnit = unit; }
		UnitT* GetTestedUnit() const { return _testedUnit; }
		UnitT* GetPathUnit(ObjT* obj) { return _getPathUnit(obj); }

		struct CheckUnitIsWithinLookupShape
		{
			FindUnitsWithinLookupShapeQuery2d* Query;
			CheckUnitIsWithinLookupShape(FindUnitsWithinLookupShapeQuery2d* query) :
				Query(query)
			{ }

			inline bool operator()(ObjT* object) const
			{
				return Query->GetTestedUnit() != Query->GetPathUnit(object) &&
					Query->GetTestedUnit()->IsBlockedBy(Query->GetPathUnit(object)) &&
					Query->GetTestedUnit()->IsWithinSight(Query->GetPathUnit(object));
			}
		};

		struct TestShape
		{
			FindUnitsWithinLookupShapeQuery2d* Query;
			TestShape(FindUnitsWithinLookupShapeQuery2d* query) :
				Query(query)
			{ }

			bool Intersect(const DbvhShapeT& dbvhShape) const
			{
				return Query->GetTestedUnit()->IsWithinSight(dbvhShape);
			}
		};

		bool Intersect(const DbvhShapeT& dbvhShape) const
		{
			return GetTestedUnit()->IsWithinSight(dbvhShape);
		}

		void Execute(Dbvh<DbvhShapeT>* dbvh)
		{
			_hitObjects.clear();
			dbvh->TestGeneric(TestShape(this), _hitObjects, CheckUnitIsWithinLookupShape(this));
		}

		PFArray<ObjT*>& GetObjectsHit() { return _hitObjects; }
	};

	// Uses given query to find lookup unit pairs in Dbvh
	template<typename DbvhShapeT, typename FindUnitsWithinLookupShapeQueryT>
	class DbvhUnitUnitLookupPairer2d : public IUnitUnitPairer<PFVector2>
	{
	public:
		typedef Dbvh<DbvhShapeT> DbvhT;

	protected:
		DbvhT* _dbvh;
		FindUnitsWithinLookupShapeQueryT _query;

	public:
		DbvhUnitUnitLookupPairer2d(DbvhT* dbvh, FindUnitsWithinLookupShapeQueryT query = FindUnitsWithinLookupShapeQueryT()) :
			IUnitUnitPairer(),
			_dbvh(dbvh),
			_query(query)
		{ }

		void FindUnitPairs()
		{
			_uuMap.clear();
			_uuMap.reserve( _units->size() );
			for(unsigned int i = 0; i < _units->size(); ++i)
			{
				UnitT* unit = (*_units)[i];
				_uuMap.push_back(std::make_pair( unit, PFArray<UnitT*>() ));
				_query.SetTestedUnit(unit);
				_query.Execute(_dbvh);
				auto& hitObjects = _query.GetObjectsHit();
				_uuMap[i].second.reserve(hitObjects.size());
				for(unsigned hit = 0; hit < hitObjects.size(); ++hit)
				{
					_uuMap[i].second.push_back(_query.GetPathUnit(hitObjects[hit]));
				}
			}
		}
	};
}