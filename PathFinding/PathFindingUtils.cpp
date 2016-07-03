#include "PathFindingUtils.h"

namespace PathFinding
{
	int FindLinesIntersectionPoint(const PFVector2& begin1, 
		const PFVector2& end1, const PFVector2& begin2, 
		const PFVector2& end2, PFVector2& intersectionPoint)
	{
		int intersectionType = IntersectionTypes::NoIntersection;
		// Vector from begin1 to begin2
		PFVector2 diffBegin(begin1.x - begin2.x, begin1.y - begin2.y);
		// Vector from end to begin ( unnormalized direction )
		PFVector2 diffEdge1(end1.x - begin1.x, end1.y - begin1.y);
		PFVector2 diffEdge2(end2.x - begin2.x, end2.y - begin2.y);
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
}