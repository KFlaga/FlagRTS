#pragma once

#include "PathingDefs.h"
#include "CollisionFilter.h"
#include <Array2d.h>

namespace FlagRTS
{
	// Obstacle on uniform grid is represented by 2d matrix
	// with obstacle filter value in each cell
	class UniformGridObstacle : public Array2d<CollisionFilter>
	{
	public:
		UniformGridObstacle(const int cellsX, const int cellsY) :
			Array2d<CollisionFilter>(cellsY, cellsX) 
		{ }
	};

}