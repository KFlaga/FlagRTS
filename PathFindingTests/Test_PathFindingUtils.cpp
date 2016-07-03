#include "stdafx.h"
#include "CppUnitTest.h"
#include <PathFindingUtils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PathFindingTests
{		
	TEST_CLASS(Test_PathFindingUtils)
	{
	public:
		TEST_METHOD(CheckVertexIsConvex_PassedConvex)
		{
			// Create dummy vertices
			PFVector2 v1(3.0f,1.0f);
			PFVector2 v2(1.0f,3.0f);
			PFVector2 v3(-4.0f,0.0f);

			Assert::IsTrue(
				PathFinding::CheckVertexIsConvex(v1,v2,v3),
				L"CheckVertexIsConvex failed to find that vetex is convex");
		}

		TEST_METHOD(CheckVertexIsConvex_PassedConcave)
		{
			// Create dummy vertices
			PFVector2 v3(3.0f,1.0f);
			PFVector2 v2(1.0f,3.0f);
			PFVector2 v1(-4.0f,0.0f);

			Assert::IsFalse(
				PathFinding::CheckVertexIsConvex(v1,v2,v3),
				L"CheckVertexIsConvex failed to find that vetex is concave");
		}

		TEST_METHOD(CheckVertexIsConvex_PassedStraight)
		{
			// Create dummy vertices
			PFVector2 v1(300.0f,0.0f);
			PFVector2 v2(200.0f,0.0f);
			PFVector2 v3(100.0f,0.0f);

			Assert::IsFalse(
				PathFinding::CheckVertexIsConvex(v1,v2,v3),
				L"CheckVertexIsConvex failed to find that vetex is straight");
		}

		TEST_METHOD(FindIntersectionPoints_NoIntersection)
		{
			// Create dummy vertices
			PFVector2 v1(300.0f,0.0f);
			PFVector2 v2(200.0f,0.0f);
			PFVector2 v3(100.0f,0.0f);
			PFVector2 v4(100.0f,0.0f);
			PFVector2 v5;

			Assert::IsTrue(
				PathFinding::FindLinesIntersectionPoint(v1,v2,v3,v4,v5) == PathFinding::IntersectionTypes::NoIntersection);
		}

		TEST_METHOD(FindIntersectionPoints_IntersectionMiddle)
		{
			// Create dummy vertices
			PFVector2 v1(30.0f,0.0f);
			PFVector2 v2(0.0f,30.0f);
			PFVector2 v3(10.0f,10.0f);
			PFVector2 v4(20.0f,40.0f);
			PFVector2 v5;

			Assert::IsTrue(
				PathFinding::FindLinesIntersectionPoint(v1,v2,v3,v4,v5) == PathFinding::IntersectionTypes::MiddlePoint);
		}

		TEST_METHOD(FindIntersectionPoints_SameBegin)
		{
			// Create dummy vertices
			PFVector2 v1(20.0f,120.0f);
			PFVector2 v2(80.0f,20.0f);
			PFVector2 v3(20.0f,120.0f);
			PFVector2 v4(100.0f,0.0f);
			PFVector2 v5;

			Assert::IsTrue(
				PathFinding::FindLinesIntersectionPoint(v1,v2,v3,v4,v5) == PathFinding::IntersectionTypes::BeginThisBeginOther);
		}

		TEST_METHOD(FindIntersectionPoints_ParallelNoIntersection)
		{
			// Create dummy vertices
			PFVector2 v1(1.0f,100.0f);
			PFVector2 v2(101.0f,0.0f);
			PFVector2 v3(0.0f,100.0f);
			PFVector2 v4(100.0f,0.0f);
			PFVector2 v5;

			Assert::IsTrue(
				PathFinding::FindLinesIntersectionPoint(v1,v2,v3,v4,v5) == PathFinding::IntersectionTypes::NoIntersection);
		}

		TEST_METHOD(FindIntersectionPoints_ParallelIntersection)
		{
			// Create dummy vertices
			PFVector2 v1(30.0f,0.0f);
			PFVector2 v2(0.0f,30.0f);
			PFVector2 v3(20.0f,10.0f);
			PFVector2 v4(50.0f,-20.0f);
			PFVector2 v5;

			Assert::IsTrue(
				PathFinding::FindLinesIntersectionPoint(v1,v2,v3,v4,v5) == PathFinding::IntersectionTypes::EdgesParallel);
		}

		TEST_METHOD(FindIntersectionPoints_Profile1mln)
		{
			// Create dummy vertices
			PFVector2 v1(30.0f,0.0f);
			PFVector2 v2(0.0f,30.0f);
			PFVector2 v3(10.0f,10.0f);
			PFVector2 v4(20.0f,40.0f);
			PFVector2 v5;

			for(unsigned int i = 0; i < 1000000u; ++i)
				PathFinding::FindLinesIntersectionPoint(v1,v2,v3,v4,v5);
		}

		TEST_METHOD(CheckVertexIsConvex_Profile1mln)
		{
			// Create dummy vertices
			PFVector2 v3(3.0f,1.0f);
			PFVector2 v2(1.0f,3.0f);
			PFVector2 v1(-4.0f,0.0f);

			for(unsigned int i = 0; i < 1000000u; ++i)
				PathFinding::CheckVertexIsConvex(v1,v2,v3);
		}
	};
}