#pragma once 

#include "PathFindingUtils.h"

namespace PathFinding
{
	class PotentialObstacle;
	class PotentialUnit;
	class UniformGridPathingMap;
	class IDebugDrawer;

	class PotentialMap
	{
	public:
		struct CellPotential
		{
			float RepulsionPart; // Potential comming from repulsion force ( common for every object )
			float GoalPart; // Potential comming from goal attraction force
		};

	private:
		PFArray2d<float> _potentialGrid;
		PFArray<PotentialUnit*> _units;
		PFArray<PotentialObstacle*> _obstacles;
		IntVector2 _cellCount;
		IntVector2 _cellDivider;
		PFVector2 _cellSize;
		PFVector2 _cellSizeInv;

	public:
		// Creates empty potential map
		PotentialMap(const IntVector2& cellCount, const IntVector2& cellDivider, const PFVector2& cellSize);

		// Adds static obstacle to map, adds its repuslive potential
		// Position is top top-left cell of obstacle
		void AddObstacle(PotentialObstacle* obstacle, const IntVector2& position);

		// Adds static obstacle to map, adds its repuslive potential
		// Position is center of obstacle
		void AddObstacle(PotentialObstacle* obstacle, const PFVector2& position);

		// Removes static obstacle from map, removes its repulsive potential
		void RemoveObstacle(PotentialObstacle* obstacle);
		
		// Adds unit to map, adds its repuslive potential to map
		void AddUnit(PotentialUnit* unit);
		// Removes unit from map, removes its repulsive potential
		void RemoveUnit(PotentialUnit* unit);
		// Updates unit repulsive potential after its move.
		// 'displacement' is vector unit moved in this frame
		void MoveUnit(PotentialUnit* unit, const IntVector2& displacement);
		// Updates unit repulsive potential after its rotation.
		// 'angle' is total rotation angle from origin
		void RotateUnit(PotentialUnit* unit, const float angle);
		// Sets units moving goal to given cell.
		// Path to this cell will be most attracive for given unit
		void SetUnitGoal(PotentialUnit* unit, const IntVector2& goalCell);

#pragma region DEBUG
		void DrawGrid(IDebugDrawer* drawer);
		void DrawObstacles(IDebugDrawer* drawer);
		void DrawUnits(IDebugDrawer* drawer);
		void DrawTotalPotentials(IDebugDrawer* drawer);
		void DrawMasksForUnit(IDebugDrawer* drawer, PotentialUnit* unit);
		void DrawMasksForObstacle(IDebugDrawer* drawer, PotentialObstacle* unit);
		void DrawPotentialForUnit(IDebugDrawer* drawer, PotentialUnit* unit);
		void DrawPotentialForObstacle(IDebugDrawer* drawer, PotentialObstacle* unit);

#pragma endregion
	};
}