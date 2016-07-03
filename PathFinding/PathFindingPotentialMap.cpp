#include "PathFindingPotentialMap.h"
#include "PathFindingPotentialObstacle.h"
#include "PathFindingIDebugDrawer.h"

namespace PathFinding
{
	PotentialMap::PotentialMap(const IntVector2& cellCount, const IntVector2& cellDivider, const PFVector2& cellSize) :
		_potentialGrid(cellCount.Y, cellCount.X),
		_cellSize(cellSize),
		_cellSizeInv(1.f/cellSize.x,1.f/cellSize.y),
		_cellDivider(cellDivider)
	{

	}

	void PotentialMap::AddObstacle(PotentialObstacle* obstacle, const IntVector2& position)
	{
		_obstacles.push_back(obstacle);
		obstacle->SetPosition(position);
		// Add its potential to map
		PFArray<PotentialObstacle::CellForce> potential = obstacle->GetRepulsiveField();
		for(unsigned int cell = 0u; cell < potential.size(); ++cell)
		{
			_potentialGrid(position + potential[cell].Shift) += potential[cell].Force;
		}
	}

	void PotentialMap::RemoveObstacle(PotentialObstacle* obstacle)
	{
		Array::RemoveElement(_obstacles, obstacle);
		IntVector2 position = obstacle->GetPosition();
		// Remove potential from map
		PFArray<PotentialObstacle::CellForce> potential = obstacle->GetRepulsiveField();
		for(unsigned int cell = 0u; cell < potential.size(); ++cell)
		{
			_potentialGrid(position + potential[cell].Shift) -= potential[cell].Force;
		}
	}

	void PotentialMap::AddUnit(PotentialUnit* unit)
	{

	}

	void PotentialMap::RemoveUnit(PotentialUnit* unit)
	{

	}

	void PotentialMap::MoveUnit(PotentialUnit* unit, const IntVector2& displacement)
	{

	}

	void PotentialMap::RotateUnit(PotentialUnit* unit, const float angle)
	{

	}

	void PotentialMap::SetUnitGoal(PotentialUnit* unit, const IntVector2& goalCell)
	{

	}

#pragma region DEBUG
	void PotentialMap::DrawGrid(IDebugDrawer* drawer)
	{
		//  __  First draw form each grid vertex (except high-boundary) :
		// |    
		// which leaves us with last horizontal/vertical lines yet to be drawn
		const int lastX = _potentialGrid.GetColumnCount() - 1;
		const int lastY = _potentialGrid.GetRowCount() - 1;
		for(int dy = 0; dy < lastY; ++dy)
		{
			for(int dx = 0; dx < lastX; ++dx)
			{
				drawer->DrawLine(
					PFVector2(dx * _cellSize.x, dy * _cellSize.y),
					PFVector2((dx+1) * _cellSize.x, dy * _cellSize.y));
				drawer->DrawLine(
					PFVector2(dx * _cellSize.x, dy * _cellSize.y),
					PFVector2(dx * _cellSize.x, (dy+1) * _cellSize.y));
			}
		}

		for(int dy = 0; dy < lastY; ++dy)
		{
			drawer->DrawLine(
				PFVector2(lastX * _cellSize.x, dy * _cellSize.y),
				PFVector2(lastX * _cellSize.x, (dy+1) * _cellSize.y));
		}

		for(int dx = 0; dx < lastX; ++dx)
		{
			drawer->DrawLine(
				PFVector2(dx * _cellSize.x, lastY * _cellSize.y),
				PFVector2((dx+1) * _cellSize.x, lastY * _cellSize.y));
		}
	}

	void PotentialMap::DrawObstacles(IDebugDrawer* drawer)
	{

	}

	void PotentialMap::DrawUnits(IDebugDrawer* drawer)
	{

	}

	void PotentialMap::DrawTotalPotentials(IDebugDrawer* drawer)
	{

	}

	void PotentialMap::DrawMasksForUnit(IDebugDrawer* drawer, PotentialUnit* unit)
	{

	}

	void PotentialMap::DrawMasksForObstacle(IDebugDrawer* drawer, PotentialObstacle* unit)
	{

	}

	void PotentialMap::DrawPotentialForUnit(IDebugDrawer* drawer, PotentialUnit* unit)
	{

	}

	void PotentialMap::DrawPotentialForObstacle(IDebugDrawer* drawer, PotentialObstacle* unit)
	{

	}

#pragma endregion
}