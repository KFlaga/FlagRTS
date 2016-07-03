#include "stdafx.h"
#include "CppUnitTest.h"
#include <PathFindingUniformGridAstarFinder.h>
#include <PathFindingUniformGridJumpPointFinder.h>
#include <PathFindingUniformGridPathingMap.h>
#include <PathFindingAstarNode.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PathFindingTests
{
	void AddObstaclesToMap(PathFinding::UniformGridPathingMap* map);

	TEST_CLASS(Test_PathFindingAstar)
	{
	public:
		TEST_METHOD(FindPathAstar_Simple_e1)
		{
			PathFinding::UniformGridPathingMap pathMap(100, 100, Ogre::Vector2(20.f,20.f), 0x02);
			PathFinding::UniformGridAstarFinder finder;
			finder.SetPathingMap(&pathMap);

			finder.SetHeuristicMultiplier(1.0f);
			finder.FindPath(10, 10, 65, 80, 1);

			float pathDist = finder.GetLastPathDistance();
			Assert::IsTrue(pathDist < 93.f, L"Optimal path is too long");
		}

		TEST_METHOD(FindPathAstar_Simple_e1p2)
		{
			PathFinding::UniformGridPathingMap pathMap(100, 100, Ogre::Vector2(20.f,20.f), 0x02);
			PathFinding::UniformGridAstarFinder finder;
			finder.SetPathingMap(&pathMap);

			finder.SetHeuristicMultiplier(1.2f);
			finder.FindPath(10, 10, 65, 80, 1);

			float pathDist = finder.GetLastPathDistance();
			Assert::IsTrue(pathDist < 93.f, L"Optimal path is too long");
		}

		TEST_METHOD(FindPathAstar_Obstacles_e1)
		{
			PathFinding::UniformGridPathingMap pathMap(100, 100, Ogre::Vector2(20.f,20.f), 0x02);
			// Add some obstacles
			AddObstaclesToMap(&pathMap);
			
			PathFinding::UniformGridAstarFinder finder;
			finder.SetPathingMap(&pathMap);
			finder.SetHeuristicMultiplier(1.0f);
			finder.FindPath(10, 10, 65, 80, 1);

			float pathDist = finder.GetLastPathDistance();
			Assert::IsTrue(pathDist < 147, L"Optimal path is too long");
		}

		TEST_METHOD(FindPathAstar_Obstacles_e1p1)
		{
			PathFinding::UniformGridPathingMap pathMap(100, 100, Ogre::Vector2(20.f,20.f), 0x02);
			// Add some obstacles
			AddObstaclesToMap(&pathMap);

			PathFinding::UniformGridAstarFinder finder;
			finder.SetPathingMap(&pathMap);
			finder.SetHeuristicMultiplier(1.05f);
			finder.FindPath(10, 10, 65, 80, 1);

			float pathDist = finder.GetLastPathDistance();
			Assert::IsTrue(pathDist < 147*1.05f, L"Optimal path is too long");
		}

		TEST_METHOD(FindPathJump_Simple)
		{
			PathFinding::UniformGridPathingMap pathMap(100, 100, Ogre::Vector2(20.f,20.f), 0x02);
			PathFinding::UniformGridJumpPointFinder finder(&pathMap);
			finder.SetBlockFilter(1);
			finder.UpdateAll();

			finder.SetHeuristicMultiplier(1.0f);

			finder.FindPath(IntVector2(10,10), IntVector2(65,80));

			float pathDist = finder.GetLastPathDistance();
			Assert::IsTrue(pathDist < 93.f, L"Optimal path is too long");
			Assert::IsTrue(pathDist > 1, L"Path not found");
		}

		TEST_METHOD(FindPathJump_Obstacles)
		{
			PathFinding::UniformGridPathingMap pathMap(100, 100, Ogre::Vector2(20.f,20.f), 0x02);
			// Add some obstacles
			AddObstaclesToMap(&pathMap);PathFinding::UniformGridJumpPointFinder finder(&pathMap);
			
			finder.SetBlockFilter(1);
			finder.UpdateAll();

			finder.SetHeuristicMultiplier(1.0f);

			finder.FindPath(IntVector2(10,10), IntVector2(65,80));

			float pathDist = finder.GetLastPathDistance();
			Assert::IsTrue(pathDist < 147*1.05, L"Optimal path is too long");
			Assert::IsTrue(pathDist > 1, L"Path not found");
		}
	};

	void AddObstaclesToMap(PathFinding::UniformGridPathingMap* map)
	{
		// Assume 100x100 cells + this call:
		//finder.FindPath(10, 10, 65, 80, 1);
		PathFinding::UniformGridObstacle wall1(3, 90);
		for(int y = 0; y < 90; ++y)
			for(int x = 0; x < 3; ++x)
				wall1.SetAt(y, x, PathFinding::CollisionFilter(1,3));
		
		map->AddObstacle(&wall1, 39, 4);

		PathFinding::UniformGridObstacle wall2(35, 3);
		for(int y = 0; y < 3; ++y)
			for(int x = 0; x < 35; ++x)
				wall2.SetAt(y, x, PathFinding::CollisionFilter(1,3));

		map->AddObstacle(&wall2, 5, 80);
		map->AddObstacle(&wall2, 41, 4);
	}
}