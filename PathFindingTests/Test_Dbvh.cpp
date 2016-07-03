#include "stdafx.h"
#include "CppUnitTest.h"
#include <PathFindingDBVH.h>
#include <PathFindingDbvhUnitUnitPairer.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PathFindingTests
{
	typedef PathFinding::BoundedPathUnit2d<> PathUnit;

	using namespace PathFinding;

	void CreateRandomDbvh(PathFinding::Dbvh<Box>& dbvh, PFArray<Box>& shapes);
	void CreateGridDbvh(PathFinding::Dbvh<Box>& dbvh, PFArray<Box>& shapes);

	// Creates 4 units which intersects ( have within sight ) 
	// with 9, 4, 2, 0 other units in dbvh
	void CreatePathUnits(PFArray<IPathUnit<PFVector2>*>& unitsArray);

	struct GetPathUnit
	{
		inline PathUnit* operator()(PathUnit* unit) { return unit; }
	};

	TEST_CLASS(Test_Dbvh)
	{
	public:
		TEST_METHOD(CreateRandomDbvh_Box)
		{
			PathFinding::Dbvh<PathFinding::Box> dbvh;
			PFArray<PathFinding::Box> objArray;
			CreateRandomDbvh(dbvh, objArray);

			dbvh.Optimize();

			Assert::IsTrue(true, L"");
		}

		TEST_METHOD(GridDbvh_PairUnitsWithDbvhUUP)
		{
			PathFinding::Dbvh<PathFinding::Box> dbvh;
			PFArray<PathFinding::Box> objArray;
			CreateGridDbvh(dbvh, objArray);

			PFArray<IPathUnit<PFVector2>*> pathUnits;
			CreatePathUnits(pathUnits);

			typedef FindUnitsWithinLookupShapeQuery2d<Box, PathUnit, GetPathUnit> QueryT;
			QueryT query;
			DbvhUnitUnitLookupPairer2d<Box, QueryT> pairer(&dbvh, query);
			pairer.SetUnitSource(&pathUnits);

			pairer.FindUnitPairs();
			auto& uumap = pairer.GetUnitPairsMap();

			Assert::IsTrue(uumap[0].second.size() == 9, L"");
			Assert::IsTrue(uumap[1].second.size() == 4, L"");
			Assert::IsTrue(uumap[2].second.size() == 2, L"");
			Assert::IsTrue(uumap[3].second.size() == 0, L"");
			Assert::IsTrue(true, L"");
		}
	};

	void CreateRandomDbvh(PathFinding::Dbvh<PathFinding::Box>& dbvh, PFArray<PathFinding::Box>& shapes)
	{
		PFArray<std::pair<PathFinding::Box*,PathFinding::Box>> dbvhArray;
		PFVector2 rangeMin(0.f,0.f);
		int rangeMax = 1000;
		int sizeMin = 10;
		int sizeMax = 100;

		for(unsigned int i = 0u; i < 50u; ++i)
		{
			PathFinding::Box box;
			shapes.push_back(box);
			box.TopLeft = PFVector2(std::rand() % rangeMax, std::rand() % rangeMax);
			box.BotRight = box.TopLeft + PFVector2(sizeMin + (std::rand() % sizeMax), sizeMin + (std::rand() % sizeMax));
		}
		for(unsigned int i = 1u; i < 50u; ++i)
		{
			dbvhArray.push_back(std::make_pair(&shapes[i], shapes[i]));
		}

		dbvh.CreateInitialBvh(dbvhArray);
	}

	void CreateGridDbvh(PathFinding::Dbvh<PathFinding::Box>& dbvh, PFArray<PathFinding::Box>& shapes)
	{

		PFArray<std::pair<PathUnit*, Box>> dbvhArray;
		PFVector2 size(100.f,100.f);
		PFVector2 spacing(50.f,50.f);
		int count = 10;
		for(unsigned int y = 0u; y < count; ++y)
		{
			for(unsigned int x = 0u; x < count; ++x)
			{
				PathUnit* unit = new PathUnit(CollisionFilter(1,1));
				Box box;
				box.TopLeft = PFVector2(x * (size.x+spacing.x) , y * (size.y+spacing.y));
				box.BotRight = box.TopLeft + size;
				unit->SetCollisionShape(box);
				unit->SetLookUpShape(box);
				shapes.push_back(box);
				dbvhArray.push_back(std::make_pair(unit, box));
			}
		}

		dbvh.CreateInitialBvh(dbvhArray);

	}

	void CreatePathUnits(PFArray<IPathUnit<PFVector2>*>& unitsArray)
	{
		PathUnit* unitCenter = new PathUnit(PathFinding::CollisionFilter(1,1));
		PathUnit* unitCornered = new PathUnit(PathFinding::CollisionFilter(1,1));
		PathUnit* unitIntersect = new PathUnit(PathFinding::CollisionFilter(1,1));
		PathUnit* unitNotIntersect = new PathUnit(PathFinding::CollisionFilter(1,1));

		// Center is center of (3,3) box
		unitCenter->SetCollisionShape(Box(PFVector2(327.5f,327.5f), PFVector2(327.5f,327.5f) + PFVector2(25.f,25.f)));
		unitCenter->SetLookUpShape(Box(PFVector2(230.f,230.f), PFVector2(230.f,230.f) + PFVector2(240.f,240.f)));

		unitCornered->SetCollisionShape(Box(PFVector2(412.5f,262.5f), PFVector2(412.5f,262.5f) + PFVector2(25.f,25.f)));
		unitCornered->SetLookUpShape(Box(PFVector2(425.f,165.f), PFVector2(425.f,165.f) + PFVector2(200.f,200.f)));

		unitIntersect->SetCollisionShape(Box(PFVector2(237.5f,487.5f), PFVector2(237.5f,487.5f) + PFVector2(25.f,25.f)));
		unitIntersect->SetLookUpShape(Box(PFVector2(150.0f, 410.0f), PFVector2(150.0f, 410.0f) + PFVector2(200.f,180.f)));

		unitNotIntersect->SetCollisionShape(Box(PFVector2(562.5f,410.5f), PFVector2(562.5f,410.5f) + PFVector2(25.f,25.f)));
		unitNotIntersect->SetLookUpShape(Box(PFVector2(562.5f,410.5f), PFVector2(562.5f,410.5f) + PFVector2(25.f,25.f)));

		unitsArray.push_back(unitCenter);
		unitsArray.push_back(unitCornered);
		unitsArray.push_back(unitIntersect);
		unitsArray.push_back(unitNotIntersect);
	}
}