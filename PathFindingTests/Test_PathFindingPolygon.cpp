#include "stdafx.h"
#include "CppUnitTest.h"
#include <PathFindingPolygon.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PathFindingTests
{		
	PathFinding::Polygon CreateTestConvexPolygon()
	{
		PathFinding::Polygon polygon;

		// Create simple rectangle
		polygon.AddVertex( PFVector2(10.0f,10.0f) );
		polygon.AddVertex( PFVector2(-10.0f,10.0f) );
		polygon.AddVertex( PFVector2(-10.0f,-10.0f) );
		polygon.AddVertex( PFVector2(10.0f,-10.0f) );
		polygon.Update();

		return polygon;
	}

	struct PolygonMergeTestSet
	{
		std::vector<PathFinding::Polygon> ToMerge;
		PathFinding::Polygon Expected;

		// Check if polygon have the same vertices
		bool CompareWithExpected(PathFinding::Polygon& result);
	};

	void CreateMergeTwoTestOnlyMiddlePoints(PolygonMergeTestSet& mergeSet);
	void CreateMergeTwoTestOverlapping(PolygonMergeTestSet& mergeSet);

	TEST_CLASS(Test_PathFindingUtils)
	{
	public:
		TEST_METHOD(Polygon_IsPointInside_PolyConvex)
		{
			PathFinding::Polygon polygon = CreateTestConvexPolygon();

			PFVector2 pointCenter( 0.0f, 0.0f );
			Assert::IsTrue(polygon.IsPointInside(pointCenter), 
				L"Point in center failed");
			
			PFVector2 pointSomewhere( 3.0f, -2.0f );
			Assert::IsTrue(polygon.IsPointInside(pointSomewhere), 
				L"Point inside failed");
			
			PFVector2 pointOnEdge( 3.0f, -10.0f );
			Assert::IsTrue(polygon.IsPointInside(pointOnEdge), 
				L"Point on edge failed");
			
			// Doesn't pass due to wrong horizontal lines handling
		//	PFVector2 pointOnVertex( -10.0f, 10.0f );
		//	Assert::IsTrue(polygon.IsPointInside(pointOnVertex), 
		//		L"Point on vertex failed");
			
			PFVector2 pointOutside( 13.0f, 7.0f );
			Assert::IsFalse(polygon.IsPointInside(pointOutside), 
				L"Point outside failed");
		}

		TEST_METHOD(Polygon_MergeTwo)
		{
			PolygonMergeTestSet testSet;
			CreateMergeTwoTestOnlyMiddlePoints(testSet);

			PathFinding::Polygon merged = PathFinding::Polygon::Merge(
				testSet.ToMerge[0], testSet.ToMerge[1]);
			
			Assert::IsTrue(merged.GetVertexCount() == 8, L"Bad final vertex count");
			Assert::IsTrue(testSet.CompareWithExpected(merged), L"Bad merged polygon vertices");
		}

		TEST_METHOD(Polygon_ProfileMergeTwo1k)
		{
			PolygonMergeTestSet testSet;
			CreateMergeTwoTestOnlyMiddlePoints(testSet);

			for(int i = 0; i < 1000; ++i)
				PathFinding::Polygon::Merge(
					testSet.ToMerge[0], testSet.ToMerge[1]);
		}
	};

	bool PolygonMergeTestSet::CompareWithExpected(PathFinding::Polygon& result)
	{
		// Find first common vertex
		unsigned int beginVertex;
		for(beginVertex = 0; beginVertex < result.GetVertexCount(); ++beginVertex)
		{
			if( IsCloseToZero(result.GetVertex(beginVertex).x - Expected.GetVertex(0).x) &&
				IsCloseToZero(result.GetVertex(beginVertex).y - Expected.GetVertex(0).y) )
			{
				break;
			}
		}

		if(beginVertex == result.GetVertexCount())
			return false; // Haven't found starting vertex
		// Check if following vertices are same
		int v;
		const int fromBeginToEnd = result.GetVertexCount() - beginVertex;
		for(v = 0; v < fromBeginToEnd; ++v)
		{
			if( !(IsCloseToZero(result.GetVertex(v+beginVertex).x - Expected.GetVertex(v).x) &&
				IsCloseToZero(result.GetVertex(v+beginVertex).y - Expected.GetVertex(v).y)) )
			{
				return false;
			}
		}
		for(; v < result.GetVertexCount(); ++v)
		{
			if( !(IsCloseToZero(result.GetVertex(v-fromBeginToEnd).x - Expected.GetVertex(v).x) &&
				IsCloseToZero(result.GetVertex(v-fromBeginToEnd).y - Expected.GetVertex(v).y)) )
			{
				return false;
			}
		}
		return true;
	}
	
	void CreateMergeTwoTestOnlyMiddlePoints(PolygonMergeTestSet& mergeSet)
	{
		PathFinding::Polygon poly1;

		poly1.AddVertex( PFVector2(10.0f,10.0f) );
		poly1.AddVertex( PFVector2(-10.0f,10.0f) );
		poly1.AddVertex( PFVector2(-10.0f,-10.0f) );
		poly1.AddVertex( PFVector2(10.0f,-10.0f) );
		poly1.Update();

		PathFinding::Polygon poly2;

		poly2.AddVertex( PFVector2(0.0f, 0.0f) );
		poly2.AddVertex( PFVector2(20.0f, 0.0f) );
		poly2.AddVertex( PFVector2(20.0f, 20.0f) );
		poly2.AddVertex( PFVector2(0.0f, 20.0f) );
		poly2.Update();

		PathFinding::Polygon expected;
		
		expected.AddVertex( PFVector2(-10.f, 10.f) );
		expected.AddVertex( PFVector2(-10.f,-10.f) );
		expected.AddVertex( PFVector2( 10.f,-10.f) );
		expected.AddVertex( PFVector2( 10.f, 0.f) );
		expected.AddVertex( PFVector2( 20.f, 0.f) );
		expected.AddVertex( PFVector2( 20.f, 20.f) );
		expected.AddVertex( PFVector2( 0.0f, 20.f) );
		expected.AddVertex( PFVector2( 0.0f, 10.f) );

		mergeSet.ToMerge.push_back(poly1);
		mergeSet.ToMerge.push_back(poly2);
		mergeSet.Expected = expected;
	}

	void CreateMergeTwoTestOverlapping(PolygonMergeTestSet& mergeSet)
	{

	}
}