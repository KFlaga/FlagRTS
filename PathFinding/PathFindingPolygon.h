#pragma once

#include "PathFindingUtils.h"
#include "PathFindingBoundingShapes.h"

namespace PathFinding
{
	// Representation of arbitrary 2d polygon ( without holes ) defined
	// by set of boundary vertices.
	// Stores vertices in array in couter-clockwise order
	class Polygon
	{
	private:
		PFArray<PFVector2> _vertices;
		Box _boundingBox;
		bool _geoChanged; // True if polygon geometry is changed since last call to Update()
		bool _isConvex;

	public:
		Polygon();
		// Reserves memory for vertices for faster insert later
		Polygon(const unsigned int finalVertexCount);
		// Creates polygon from given array of vertices ( must be in counter-clockwise order )
		Polygon(const PFArray<PFVector2>& vertices);

		// Updates some geometry info -> bounding box
		void Update();

		// Removes all vertices from polygon
		void ClearVertices()
		{
			_vertices.clear();
			_geoChanged = true;
		}
		// Adds vertex to polygon ( must be in counter-clockwise order )
		void AddVertex(const PFVector2& vertex)
		{
			_vertices.push_back(vertex);
			_geoChanged = true;
		}
		// Reserves memory for vertices if their count is known prior to adding them
		void ReserveVertexCount(const unsigned int count)
		{
			_vertices.reserve(count);
		}
		// Insert vertex to polygon on given position ( must be in counter-clockwise order )
		void InsertVertex(const unsigned int idx, const PFVector2& vertex);
		// Removes vertex on given position
		void RemoveVertex(const unsigned int idx);

		// Returns vertex on given position
		const PFVector2& GetVertex(const unsigned int idx) const
		{
			return _vertices[idx];
		}

		unsigned int GetVertexCount() const 
		{
			return _vertices.size();
		}

		// Sets new value for given vertex
		void EditVertex(const unsigned int idx, const PFVector2& vertex)
		{
			_vertices[idx] = vertex;
			_geoChanged = true;
		}

		// Check if given point is inside polygon
		bool IsPointInside(const PFVector2& point);

		// Returns true if polygon is convex
		bool IsConvex();
		
		// Check if this polygon intersects with other
		bool DoesIntersect(const Polygon& other);

		
	//	// Merges 2 polygons( vertices inside boundaries 
	//	// are removed ). If there is no intersection, then copy
	//	// of first polygon is returned
		static Polygon Merge(const Polygon& first, const Polygon& second);
	};
}