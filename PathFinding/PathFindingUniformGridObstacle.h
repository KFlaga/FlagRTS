#pragma once

#include "PathFindingUtils.h"

namespace PathFinding
{
	// Obstacle on uniform grid is represented by 2d matrix
	// with obstacle filter value in each cell
	class UniformGridObstacle : public PFArray2d<CollisionFilter>
	{
	public:
		UniformGridObstacle(const int cellsX, const int cellsY) :
			Array2d(cellsY, cellsX) 
		{ }
	};

}