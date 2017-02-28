#pragma once

#include "PathingDefs.h"

namespace FlagRTS
{
	// Check if vertex between 2 other vertices ( middle one ) is convex 
	// ( vertex passed in counter-clockwise order create triangle :
	// if angle inside this triangle is <= 180 it is convex )
	inline bool CheckVertexIsConvex(
		const Vector2& begin,
		const Vector2& middle, 
		const Vector2& end)
	{
		// First find 2 edges' unnormalized directions :
		// edge1 = middle - begin;
		// edge2 = end - middle;
		// Edge i is in form ( di * cos(ai), di * sin(ai) ) = di(cos(ai),sin(ai))
		// To check if angle is <= 180 first we need to transform
		// edge2 to coordinate system of edge1 ( that is rotate edge 2 by -a1 )
		// Rotation by -a1 will give :
		// x2' = x2 * cos(a1) + y2 * sin(a1)
		// y2' = -x2 * sin(a1) + y2 * cos(a1)
		// Now, if y2 > 0 then 0 < a2 - a1 < 180 ( that is edge2 is
		// 'above' edge1 and so can create valid triangle )
		// Becouse scale doesn't matter ( only sign, and di > 0), so instead 
		// of cos1,sin1 - x1, y1 can be used, giving final check :
		// if( -(x2*y1) + (y2*x1) > 0) then is convex
		// becouse x1 = middle.x - begin.x, x2 = end.x - middle.x and so on, 
		// one line check is:
		float result = ( (end.y - middle.y)*(middle.x - begin.x) -
			(end.x - middle.x)*(middle.y - begin.y) ) ;
		return result > 0.0f;
	}

	// Line segment ( edges ) intersection point positions
	namespace IntersectionTypes
	{
		enum _IT
		{
			NoIntersection = 0, // Edges do not intersect
			MiddlePoint = 1, // Edges intersect between vertices of both edges
			VertexBeginThis = 2, // Edges intersect : 'this' ( on which test is called ) on begin vertex, other one in the middle
			VertexEndThis = 4, // Edges intersect : 'this' ( on which test is called ) on end vertex, other one in the middle
			VertexBeginOther = 8, // Edges intersect : 'other' ( one passed as test param ) on begin vertex, other one in the middle
			VertexEndOther = 16, // Edges intersect : 'other' ( one passed as test param ) on end vertex, other one in the middle
			BeginThisBeginOther = VertexBeginThis | VertexBeginOther, // Both edges intersect on vertices
			BeginThisEndOther = VertexBeginThis | VertexEndOther, // Both edges intersect on vertices
			EndThisBeginOther = VertexEndThis | VertexBeginOther, // Both edges intersect on vertices
			EndThisEndOther = VertexEndThis | VertexEndOther, // Both edges intersect on vertices
			EdgesParallel = 32, // Edges are parallel ( but intersect )
		};
	}
	typedef IntersectionTypes::_IT IntersectionType;

	// Finds intersection point of two line segments defined by their begin/end
	// Returns type of intersection and point of intersection if such occurs
	int FindLinesIntersectionPoint(const Vector2& begin1, 
		const Vector2& end1, const Vector2& begin2, 
		const Vector2& end2, Vector2& intersectionPoint)
	{
		int intersectionType = IntersectionTypes::NoIntersection;
		// Vector from begin1 to begin2
		Vector2 diffBegin(begin1.x - begin2.x, begin1.y - begin2.y);
		// Vector from end to begin ( unnormalized direction )
		Vector2 diffEdge1(end1.x - begin1.x, end1.y - begin1.y);
		Vector2 diffEdge2(end2.x - begin2.x, end2.y - begin2.y);
		// Cross product of directional vectors
		float crossProd = diffEdge1.crossProduct(diffEdge2);
		if(IsCloseToZero(crossProd))
		{
			// Parallel edges
			// If diffBegin.y/diffBegin.x = _diffVec.y/_diffVec.x - they lie on same line
			if( IsCloseToZero(diffBegin.y * diffEdge1.x - diffEdge1.y * diffBegin.x) )
			{
				// They lie on same line
				// Now if this.begin.x lies within other x bounds or this.end.x does
				std::pair<float,float> xbounds1 = 
					begin1.x < end1.x ?
					std::make_pair(begin1.x, end1.x) :
					std::make_pair(end1.x, begin1.x);

				std::pair<float,float> xbounds2 = 
					begin2.x < end2.x ?
					std::make_pair(begin2.x, end2.x) :
					std::make_pair(end2.x, begin2.x);

				if( xbounds1.first > xbounds2.second ||
					xbounds1.second < xbounds2.first ) { }
				else
					intersectionType = IntersectionTypes::EdgesParallel;
			}
			return intersectionType;
		}
		float crossProdInv = 1.0f / crossProd;
		// Get ratios where line intersection point lies (begin + ratio*diff)
		float ratioThis = (diffEdge1.x * diffBegin.y - diffEdge1.y * diffBegin.x) * crossProdInv;
		float ratioOther = (-diffEdge2.y * diffBegin.x + diffEdge2.x * diffBegin.y ) * crossProdInv;
		// If both ratios are (0,1) then point lies on edges
		intersectionPoint = begin1 + diffEdge1 * ratioThis;
		if( ratioThis + RELATIVE_ERROR > 0.0f && 
			ratioThis - RELATIVE_ERROR < 1.0f &&
			ratioOther + RELATIVE_ERROR > 0.0f && 
			ratioOther - RELATIVE_ERROR < 1.0f )
		{
			if( IsCloseToZero(ratioThis) )
				intersectionType |= IntersectionTypes::VertexBeginThis;
			else if( IsCloseToZero(ratioThis - 1.0f) )
				intersectionType |= IntersectionTypes::VertexEndThis;
			if( IsCloseToZero(ratioOther) )
				intersectionType |= IntersectionTypes::VertexBeginOther;
			else if( IsCloseToZero(ratioOther - 1.0f) )
				intersectionType |= IntersectionTypes::VertexEndOther;
			if(intersectionType == IntersectionTypes::NoIntersection)
				intersectionType = IntersectionTypes::MiddlePoint;
		}
		return intersectionType;
	}

	namespace Array
	{
		template<typename T>
		inline void InsertElement(PFArray<T>& table, const unsigned int index, const T& element)
		{
			table.push_back(table[table.size() - 1]);
			for(unsigned int i = table.size() - 1; i > index; --i)
			{
				table[i] = table[i+1];
			}
			table[idx] = element;
		}

		template<typename T>
		void RemoveElement(PFArray<T>& table, const T& element)
		{
			unsigned int i = 0u;
			for(; i < table.size(); ++i)
			{
				if(table[i] == element)
					break;
			}
			if(i < table.size())
			{
				for(; i < table.size()-1; ++i)
				{
					table[i] = table[i+1];
				}
				table.pop_back();
			}
		}

		template<typename T>
		void RemoveIndex(PFArray<T>& table, const unsigned int index)
		{
			for(unsigned int i = index; i < table.size(); ++i)
			{
				table[i] = table[i+1];
			}
			table.pop_back();
		}

		template<typename T>
		int FindIndex(PFArray<T>& table, const T& element)
		{
			for(unsigned int i = 0u; i < table.size(); ++i)
			{
				if(table[i] == element)
					return i;
			}
			return -1;
		}
	}
}