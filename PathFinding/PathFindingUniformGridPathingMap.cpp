#include "PathFindingUniformGridPathingMap.h"
#include "PathFindingIDebugDrawer.h"

namespace PathFinding
{
	UniformGridPathingMap::UniformGridPathingMap(const int cellCountX, 
		const int cellCountY, 
		const PFVector2& cellSize, 
		const int standardCellValue) :
	_cellSize(cellSize),
		_cellSizeInv(1.0f/cellSize.x, 1.0f/cellSize.y),
		_stdCellValue(standardCellValue),
		_grid(cellCountY, cellCountX)
	{
		_grid.Fill(standardCellValue);
	}

	void UniformGridPathingMap::AddObstacle(UniformGridObstacle* obstacle, const int px, const int py)
	{
		for(dy = 0; dy < obstacle->GetRowCount(); ++dy)
		{
			for(dx = 0; dx < obstacle->GetColumnCount(); ++dx)
			{
				_grid(py+dy, px+dx).Value |= obstacle->GetCopyAt(dy,dx).Value;
			}
		}
	}

	void UniformGridPathingMap::RemoveObstacle(UniformGridObstacle* obstacle, const int px, const int py)
	{
		for(dy = 0; dy < obstacle->GetRowCount(); ++dy)
		{
			for(dx = 0; dx < obstacle->GetColumnCount(); ++dx)
			{
				_grid(py+dy, px+dx).Value &= ~(obstacle->GetCopyAt(dy,dx).Value);
			}
		}
	}

	bool UniformGridPathingMap::CheckObstacleCanBePlaced(UniformGridObstacle* obstacle, const int px, const int py)
	{
		int blocked = 0;
		for(dy = 0; dy < obstacle->GetRowCount(); ++dy)
		{
			for(dx = 0; dx < obstacle->GetColumnCount(); ++dx)
			{
				blocked |= _grid(px+dx, py+dy).Blocking & obstacle->GetAt(dy,dx).Groups;
			}
		}
		return blocked != 0;
	}

	void UniformGridPathingMap::FindTopLeftOfObstacleWithCenterPosition(UniformGridObstacle* obstacle, const PFVector2& position, int* x, int* y)
	{
		if((obstacle->GetRowCount() & 1) > 0) // odd
		{
			*y = (int)((position.y * _cellSizeInv.y) - (obstacle->GetRowCount() - 1) * 0.5f); // Find center cell and move half-1 cells left
		}
		else // even
		{
			float yy = position.y * _cellSizeInv.y;
			float remY = yy - (int)yy;
			*y = (int)(yy - obstacle->GetRowCount() * 0.5f); 
		}
		if((obstacle->GetColumnCount() & 1) > 0) // odd
		{
			*x = (int)((position.x * _cellSizeInv.x) - (obstacle->GetColumnCount() - 1) * 0.5f);
		}
		else // even
		{
			float xx = position.x * _cellSizeInv.x;
			float remX = xx - (int)xx;
			*x = (int)(xx - obstacle->GetColumnCount() * 0.5f); 
		}	
	}

	void UniformGridPathingMap::DrawGrid(IDebugDrawer* drawer)
	{
		//  __  First draw form each grid vertex (except high-boundary) :
		// |    
		// which leaves us with last horizontal/vertical lines yet to be drawn
		const int lastX = _grid.GetColumnCount() - 1;
		const int lastY = _grid.GetRowCount() - 1;
		for(dy = 0; dy < lastY; ++dy)
		{
			for(dx = 0; dx < lastX; ++dx)
			{
				drawer->DrawLine(
					PFVector2(dx * _cellSize.x, dy * _cellSize.y),
					PFVector2((dx+1) * _cellSize.x, dy * _cellSize.y));
				drawer->DrawLine(
					PFVector2(dx * _cellSize.x, dy * _cellSize.y),
					PFVector2(dx * _cellSize.x, (dy+1) * _cellSize.y));
			}
		}

		for(dy = 0; dy < lastY; ++dy)
		{
			drawer->DrawLine(
				PFVector2(lastX * _cellSize.x, dy * _cellSize.y),
				PFVector2(lastX * _cellSize.x, (dy+1) * _cellSize.y));
		}

		for(dx = 0; dx < lastX; ++dx)
		{
			drawer->DrawLine(
				PFVector2(dx * _cellSize.x, lastY * _cellSize.y),
				PFVector2((dx+1) * _cellSize.x, lastY * _cellSize.y));
		}
	}

	void UniformGridPathingMap::DrawObstacles(IDebugDrawer* drawer, int blockedGroups)
	{
		// For now just put X if cell is ocupied
		for(dy = 0; dy < _grid.GetRowCount()-1; ++dy)
		{
			for(dx = 0; dx < _grid.GetColumnCount()-1; ++dx)
			{
				if(_grid(dy,dx).BlocksUnit(CollisionFilter(blockedGroups, blockedGroups)))
				{
					drawer->DrawLine(
						PFVector2(dx * _cellSize.x, dy * _cellSize.y),
						PFVector2((dx+1) * _cellSize.x, (dy+1) * _cellSize.y));

					drawer->DrawLine(
						PFVector2((dx+1) * _cellSize.x, dy * _cellSize.y),
						PFVector2(dx * _cellSize.x, (dy+1) * _cellSize.y));
				}
			}
		}
	}
}