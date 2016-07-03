#pragma once

#include "PathFindingDbvh.h"

namespace PathFinding
{
	template<typename ObjT>
	struct AcceptAllDyscryminator
	{
		inline bool operator()(ObjT* object)
		{
			return true;
		}
	};

	template<typename TestShapeT, 
		typename DbvhShapeT, 
		typename ObjT>
	class DbvhQuery
	{
	protected:
		TestShapeT _testShape;
		PFArray<ObjT*> _hitObjects;

	public:
		DbvhQuery() { }
		DbvhQuery(const TestShapeT& shape) :
			_testShape(shape)
		{ }

		void SetTestShape(const TestShapeT& shape)
		{
			_testShape = shape;
		}

		const TestShapeT& GetTestShape() const
		{
			return _testShape;
		}

		bool Intersect(const DbvhShapeT& dbvhShape)
		{
			return _testShape.Intersect(dbvhShape);
		}
		
		template<typename Dysryminator = AcceptAllDyscryminator<ObjT>>
		void Execute(Dbvh<DbvhShapeT>* dbvh, Dysryminator& acceptHit = Dysryminator())
		{
			_hitObjects.clear();
			dbvh->TestGeneric(_testShape, _hitObjects, acceptHit);
		}

		PFArray<ObjT*>& GetObjectsHit() { return _hitObjects; }
	};
}