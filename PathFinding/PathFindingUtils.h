#pragma once

#include "PathFindingPreReqs.h"

namespace PathFinding
{
	// Check if vertex between 2 other vertices ( middle one ) is convex 
	// ( vertex passed in counter-clockwise order create triangle :
	// if angle inside this triangle is <= 180 it is convex )
	inline bool CheckVertexIsConvex(
		const PFVector2& begin,
		const PFVector2& middle, 
		const PFVector2& end)
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
#ifdef _DEBUG
		float result = ( (end.y - middle.y)*(middle.x - begin.x) -
			(end.x - middle.x)*(middle.y - begin.y) ) ;
#endif
		return ( (end.y - middle.y)*(middle.x - begin.x) -
			(end.x - middle.x)*(middle.y - begin.y) ) > 0.0f;
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
	int FindLinesIntersectionPoint(const PFVector2& begin1, 
		const PFVector2& end1, const PFVector2& begin2, 
		const PFVector2& end2, PFVector2& intersectionPoint);

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

	struct CollisionFilter
	{
		union 
		{
			int Value;
			struct 
			{
				short Groups; // Obstacle groups this cell belongs to ( multiple ORed groups )
				short Blocking; // Obstacle groups this cell is blocking ( multiple ORed groups )
							   // Obstacle can be placed only if there is Groups & Blocking == 0
			};
		};
		CollisionFilter() : Value(0) { }
		CollisionFilter(const int value) : Value(value) { }
		CollisionFilter(const short groups, const short blocks) : Groups(groups), Blocking(blocks) { }
		operator int() const { return Value; }

		bool BlocksUnit(const CollisionFilter otherUnitsFilter) const
		{
			return (this->Blocking & otherUnitsFilter.Groups) != 0;
		}
		
		bool IsBlockedBy(const CollisionFilter otherUnitsFilter) const
		{
			return (this->Groups & otherUnitsFilter.Blocking) != 0;
		}

		void operator+=(CollisionFilter filter2)
		{
			Groups |= filter2.Groups;
			Blocking |= filter2.Blocking;
		}
		
		void operator-=(CollisionFilter filter2)
		{
			Groups &= ~(filter2.Groups);
			Blocking &= ~(filter2.Blocking);
		}
	};
}