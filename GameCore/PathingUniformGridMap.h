#pragma once

#include "PathingUniformGridObstacle.h"
#include "PathingDrawer.h"

namespace FlagRTS
{
	// Map made of uniform sized cells, each containg info on placed obstacle
	// groups and groups that are blocked
	// Cell count in one dimension should not exceed short capacity
	class UniformGridPathingMap
	{
		Array2d<CollisionFilter> _grid;
		int dx, dy;
		Vector2 _cellSize;
		Vector2 _cellSizeInv;
		CollisionFilter _stdCellValue;
		bool _mapChanged; // set to true if some obstacles are added/removed since last call to Update

	public:
		// Creates empty map with given cells each with given size. Map is zeroed with 'standardCellValue'
		UniformGridPathingMap(const int cellCountX, const int cellCountY, const Vector2& cellSize, 
			const int standardCellValue);

		// Adds obstacle to map on given cell
		void AddObstacle(UniformGridObstacle* obstacle, const int posX, const int posY);
		// Adds obstacle to map with center of its footprint on given position ->
		// if FP have odd number of cells in row/col then cell 'centerPosition' falls into is choosen as center cell
		// if FP have even number then nearest edge is choosen as center of FP
		void AddObstacle(UniformGridObstacle* obstacle, const Vector2& centerPosition)
		{
			int x,y;
			FindTopLeftOfObstacleWithCenterPosition(obstacle, centerPosition, &x, &y);
			AddObstacle(obstacle, x, y);
		}

		// Removes obstacle from map on given cell
		void RemoveObstacle(UniformGridObstacle* obstacle, const int posX, const int posY);
		// Adds obstacle to map with center of its footprint on given position
		void RemoveObstacle(UniformGridObstacle* obstacle, const Vector2& centerPosition)
		{
			int x,y;
			FindTopLeftOfObstacleWithCenterPosition(obstacle, centerPosition, &x, &y);
			RemoveObstacle(obstacle, x, y);
		}

		// Removes all obstacles
		void Clear()
		{
			_grid.Fill(_stdCellValue);
		}

		// Returns obstacle filter on given cell
		CollisionFilter GetCell(const int y, const int x) const	{ return _grid(y, x); }

		// Returns obstacle filter on given cell
		CollisionFilter& GetCell(const int y, const int x) { return _grid(y, x); }

		// Returns obstacle filter on given cell
		CollisionFilter GetCell(const IntVector2& pos) const { return _grid(pos.Y, pos.X); }

		// Returns obstacle filter on given cell
		CollisionFilter& GetCell(const IntVector2& pos) { return _grid(pos.Y, pos.X); }

		int GetCellCountX() const { return _grid.GetColumnCount(); }

		int GetCellCountY() const {	return _grid.GetRowCount();	}

		const Vector2 GetCellSize() const { return _cellSize; }

		bool MapChanged() const { return _mapChanged; }
		void Update();

		// Checks if all cells of obstacle's footprint are free for it group
		bool CheckObstacleCanBePlaced(UniformGridObstacle* obstacle, const int x, const int y);

		void FindCellPositionFallsInto(const Vector2& position, int* x, int* y)
		{
			*x = (int)(position.x * _cellSizeInv.x);
			*y = (int)(position.y * _cellSizeInv.y);
		}

		IntVector2 FindCellPositionFallsInto(const Vector2& position)
		{
			return IntVector2((int)(position.x * _cellSizeInv.x), (int)(position.y * _cellSizeInv.y));
		}

		void FindTopLeftOfObstacleWithCenterPosition(UniformGridObstacle* obstacle, const Vector2& position, int* x, int* y);
		IntVector2 FindTopLeftOfObstacleWithCenterPosition(UniformGridObstacle* obstacle, const Vector2& position)
		{
			IntVector2 topLeft;
			FindTopLeftOfObstacleWithCenterPosition(obstacle, position, &topLeft.X, &topLeft.Y);
			return topLeft;
		}

		void DrawGrid(PathingDrawer* drawer);
		void DrawObstacles(PathingDrawer* drawer, int blockedGroups);
	};
}