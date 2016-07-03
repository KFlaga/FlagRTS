#pragma once

#include "PathFindingPolygon.h"

namespace PathFinding
{
	// Set of separate Polygons
	class PolygonSet
	{
	private:
		PFArray<Polygon*> _polygons;

	public:
		PolygonSet();
		~PolygonSet();

		void MergeIntoSet(const Polygon& polygon);
		void RemoveFromSet(const Polygon& polygon);

		unsigned int GetSeparatePolygonCount() const
		{
			return _polygons.size();
		}

		Polygon* GetPolygon(const unsigned int num) const
		{
			return _polygons[num];
		}

	};
}