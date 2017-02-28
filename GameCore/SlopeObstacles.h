#pragma once 

#include "PathingDefs.h"
#include "CollisionFilter.h"
#include <Array.h>
#include <Array2d.h>

namespace FlagRTS

{
	class UniformGridPathingMap;

	struct SlopeRule
	{
		CollisionFilter Filter;
		float MinAngle;
		float MaxAngle;
	};

	// Fills pathing map with obstacles based on supplied heightmap and rules
	// For each cell it checks its 4 vertices and finds greatest angle between them.
	// Based on angle sets CollisionFilter of cell
	class HeightMapConverter
	{
	private:
		UniformGridPathingMap* _targetMap;
		Array<SlopeRule> _rules;
		Array2d<float>* _heightMap;

	public:
		HeightMapConverter();

		// Sets pathing and hight map
		// In height map each cell is a vertex, for one cell in pathing map (x,y)
		// theres 4 vertices : (x,y), (x+1,y), (x,y+1), (x+1,y+1)
		// Therefore size of heightmap must be pathing map size + 1 in each dimension
		void SetMap(UniformGridPathingMap* pathingMap, 
			Array2d<float>* heightMap);

		void AddSlopeRule(const SlopeRule& rule);
		// Removes all SlopeRules
		void ClearRules();

		// Adds obstacles to PathingMap based on given height map and SlopeRule set
		void ComputeObstacles();
	};
}