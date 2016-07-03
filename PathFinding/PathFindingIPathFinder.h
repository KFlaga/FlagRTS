#pragma once

#include "PathFindingUtils.h"
#include "PathFindingUniformGridObstacle.h"

namespace PathFinding
{
	class IGlobalPathFinder
	{
	protected:
		PFArray<IntVector2> _lastPath;
		PFArray<PFVector2> _lastPathReal;
		CollisionFilter _filter;
		IntVector2 _unitSize;
		bool _unitSquare;

	public:
		IGlobalPathFinder() { }
		virtual ~IGlobalPathFinder() { }

		// Change to something that will also depend on unit size (like min free cell count)
		virtual void FindPath(const IntVector2& beginCell, const IntVector2& endCell) = 0;

		virtual void SetBlockFilter(const CollisionFilter filter) { _filter = filter; }

		virtual void SetUnitSize(const IntVector2& sizeInCells) 
		{ 
			_unitSize = sizeInCells; 
			_unitSquare = (_unitSize.X == _unitSize.Y);
		}

		// To be called when cell's collision filter changes
		virtual void UpdateCell(int cellX, int cellY) { }
		void UpdateCell(const IntVector2& cell)
		{
			UpdateCell(cell.X, cell.Y);
		}

		// To be called when obstacle collision filter changes (obstacle added/removed)
		virtual void UpdateObstacle(const PFArray2d<CollisionFilter>& obstacle, const IntVector2& topLeftCell) { }

		// To be called when many cells on map changed
		virtual void UpdateAll() { }

		// Points in path are cells occupied by center cell of unit
		// Returned path starts from goal, so first index is goal cell, last is start cell 
		// For even number of cells in given dimension, center is on negative side of real center
		// ( negative in map coords ), for odd, its just center cell
		// 
		PFArray<IntVector2>& GetLastPath() { return _lastPath; }

		// Returns path points in real map coords, not cells
		PFArray<PFVector2>& GetLastPathReal() { return _lastPathReal; }
	};

	template<typename VectorT>
	class IPathUnit;

	template<typename VectorT>
	class ILocalPathFinder
	{
	public:
		ILocalPathFinder() { }
		virtual ~ILocalPathFinder() { }

		virtual void FindUnitMoves(float lastFrameMs) = 0;

		virtual void AddUnit(IPathUnit<VectorT>* unit) {}
		virtual void RemoveUnit(IPathUnit<VectorT>* unit) {}

		virtual void MoveUnit(IPathUnit<VectorT>* unit) {}
		virtual void RotateUnit(IPathUnit<VectorT>* unit) {}
	};
}