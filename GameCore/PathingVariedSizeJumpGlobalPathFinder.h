
#pragma once

#include "PathingJumpGlobalPathFinder.h"

namespace FlagRTS
{
	// Unlike normal JumpPointFinder it can handle varius unit sizes
	// Unit can occupy few cells (rectangular shape) and if all cells are free
	// then unit can move there
	// Cells occupied by unit are rotated around center to match movement direction
	// It may take much more time as it needs to dynamically compute if cells are free
	// It does not take into account space unit may need to rotate from one direction to another
	// For now it may crash if unit moves near border of map
	class UniformGridVariedSizeJumpFinder : public UniformGridJumpPointFinder
	{
	protected:
		struct UnitMask
		{
			IntVector2 Size; // AABB size of mask
			IntVector2 Center; // Center cell, assuming top-left is (0,0)
			Array<IntVector2> Cells; // All cells in mask, relative to center cell

			IntVector2& operator[](const int idx)
			{
				return Cells[idx];
			}
		};

		UnitMask* _masks[11];
		std::map<int, Array2d<int>*> _perMaskCodesMap;
		std::map<int, UnitMask> _masksMap;

	public:
		UniformGridVariedSizeJumpFinder(UniformGridPathingMap* pathMap);
		~UniformGridVariedSizeJumpFinder();

		void SetUnitSize(const IntVector2& sizeInCells) 
		{ 
			IGlobalPathFinder::SetUnitSize(sizeInCells);
			ComputeUnitMasks();
		}

		void UpdateAll();

		void UpdateCell(int cellX, int cellY);

	protected:
		void RecosntructPath(AstarNode* startNode, AstarNode* goalNode);

		IntVector2 JumpStraight(const IntVector2& from, const IntVector2& direction, const IntVector2& goal);
		IntVector2 JumpDiagonal(const IntVector2& from, const IntVector2& direction, const IntVector2& goal);
		void FindSuccessors(const IntVector2& current, 
			const IntVector2& parent, 
			const IntVector2& goal, 
			Array<IntVector2>& sucessors);

		int CheckMaskIsBlocked(int left, int top, UnitMask& mask);
		void ComputeUnitMasks();
		void ComputeUnitMasksForStraightMoves();
		void ComputeUnitMasksForDiagonalMoves();
		int ComputeMaskedPatchCode(int jx, int jy, UnitMask&);
		void ComputeCellCodesMapForMask(int dirCode);
	};
}