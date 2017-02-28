#include "PathingVariedSizeJumpGlobalPathFinder.h"
#include "PathingUniformGridMap.h"
#include "PathingAStarNodeQueue.h"
#include <map>
#include <Profiler.h>

namespace FlagRTS
{
	int CodeMask(int dirCode, IntVector2& unitSize)
	{
		// Mask code:
		// bits: 0       3 4        16 17       29
		//	     [DirCode] [UnitSizeX] [UnitSizeY]
		return dirCode | ((unsigned int)(unitSize.X) << 4) | ((unsigned int)(unitSize.X) << 17);
	}

	UniformGridVariedSizeJumpFinder::UniformGridVariedSizeJumpFinder(UniformGridPathingMap* pathMap) :
		UniformGridJumpPointFinder(pathMap)
	{

	}

	UniformGridVariedSizeJumpFinder::~UniformGridVariedSizeJumpFinder()
	{
		for(auto codeMapIt = _perMaskCodesMap.begin(); 
			codeMapIt != _perMaskCodesMap.end(); ++codeMapIt)
		{
			delete codeMapIt->second;
		}
	}

	int UniformGridVariedSizeJumpFinder::CheckMaskIsBlocked(int x, int y, UnitMask& mask)
	{
		for(unsigned int i = 0; i < mask.Cells.size(); ++i)
		{
			if( (_pathingMap->GetCell(y + mask[i].Y, x + mask[i].X)
				.Blocking & _filter.Groups) > 0 )
			{
				return 1;
			}
		}
		return 0;
	}

	int UniformGridVariedSizeJumpFinder::ComputeMaskedPatchCode(int jx, int jy, UnitMask& mask)
	{
		PROFILE_REGISTER_CLOCK(maskClock, "ComputeMaskedPatchCode");
		PROFILE_START(maskClock);
		// For each cell in 3x3 patch find if there's obstacle on mask
		int cellCode = 0;
		cellCode |= CheckMaskIsBlocked(jx-1, jy-1, mask) << 0;
		cellCode |= CheckMaskIsBlocked(jx, jy-1, mask) << 1;
		cellCode |= CheckMaskIsBlocked(jx+1, jy-1, mask) << 2;
		cellCode |= CheckMaskIsBlocked(jx-1, jy, mask) << 3;
		cellCode |= CheckMaskIsBlocked(jx, jy, mask) << 4;
		cellCode |= CheckMaskIsBlocked(jx+1, jy, mask) << 5;
		cellCode |= CheckMaskIsBlocked(jx-1, jy+1, mask) << 6;
		cellCode |= CheckMaskIsBlocked(jx, jy+1, mask) << 7;
		cellCode |= CheckMaskIsBlocked(jx+1, jy+1, mask) << 8;
		PROFILE_END(maskClock);
		return cellCode;
	}

	IntVector2 UniformGridVariedSizeJumpFinder::JumpStraight(
		const IntVector2& from, const IntVector2& direction, const IntVector2& goal)
	{
		JumpResult jumpResult = ClearCell;
		IntVector2 jumpPoint(from);
		int cellCode; // Code depends on obstacles on patch
		int dirCode = CodeDirection(direction.X, direction.Y);

		while(true)
		{
			jumpPoint += direction;

			cellCode = _perMaskCodesMap[CodeMask(dirCode, _unitSize)]->GetAt(
				jumpPoint.Y, jumpPoint.X);

			jumpResult = _resultMap(cellCode, dirCode).Result;

			// Reached goal or forced neighbour -> this is jump point
			if(jumpResult == ForcedNeighbour ||
				jumpPoint == goal )
				return jumpPoint;

			// Reached obstacle, so no jump point added
			if(jumpResult == Obstacle)
				return IntVector2(-1,-1);		
		}
		return IntVector2(-1,-1);
	}

	IntVector2 UniformGridVariedSizeJumpFinder::JumpDiagonal(
		const IntVector2& from, const IntVector2& direction, const IntVector2& goal)
	{
		JumpResult jumpResult = ClearCell;
		JumpResult farjumpResult = ClearCell;
		IntVector2 jumpPoint(from);
		IntVector2 farJumpPoint;
		IntVector2 xDir(direction.X,0);
		IntVector2 yDir(0,direction.Y);
		int cellCode; // Code depends on obstacles on patch
		int dirCode = CodeDirection(direction.X, direction.Y);
		int xDirCode = CodeDirection(xDir.X, xDir.Y);
		int yDirCode = CodeDirection(yDir.X, yDir.Y);

		while(true)
		{
			jumpPoint += direction;

			cellCode = _perMaskCodesMap[CodeMask(dirCode, _unitSize)]->GetAt(
				jumpPoint.Y, jumpPoint.X);

			jumpResult = _resultMap(cellCode, dirCode).Result;

			// Reached goal or forced neighbour -> this is jump point
			if(jumpResult == ForcedNeighbour ||
				jumpPoint == goal )
				return jumpPoint;

			// Reached obstacle, so no jump point added
			if(jumpResult == Obstacle)
				return IntVector2(-1,-1);

			farJumpPoint.X = jumpPoint.X;
			farJumpPoint.Y = jumpPoint.Y;
			while(true)
			{
				farJumpPoint += xDir;
				cellCode = _perMaskCodesMap[CodeMask(dirCode, _unitSize)]->GetAt(
					farJumpPoint.Y, farJumpPoint.X);

				farjumpResult = _resultMap(cellCode, xDirCode).Result;

				if(farjumpResult == ForcedNeighbour ||
					farJumpPoint == goal )
					return jumpPoint;

				if(farjumpResult == Obstacle)
					break;
			}
			farJumpPoint.X = jumpPoint.X;
			farJumpPoint.Y = jumpPoint.Y;
			while(true)
			{
				farJumpPoint += yDir;
				cellCode = _perMaskCodesMap[CodeMask(dirCode, _unitSize)]->GetAt(
					farJumpPoint.Y, farJumpPoint.X);

				farjumpResult = _resultMap(cellCode, yDirCode).Result;

				if(farjumpResult == ForcedNeighbour ||
					farJumpPoint == goal )
					return jumpPoint;

				if(farjumpResult == Obstacle)
					break;
			}

		}
		return IntVector2(-1,-1);
	}

	inline void UniformGridVariedSizeJumpFinder::FindSuccessors(const IntVector2& current, 
		const IntVector2& parent, const IntVector2& goal, 
		Array<IntVector2>& successors)
	{
		PROFILE_REGISTER_CLOCK(maskClock, "FindSuccessors");
		PROFILE_START(maskClock);
		int dirCode = FindMoveDirCode(current, parent);
		// Find cell code for such rotated unit
		int cellCode = _perMaskCodesMap[CodeMask(dirCode, _unitSize)]->GetAt(
			current.Y, current.X);

		Array<IntVector2>& neighboursDirs = _resultMap(cellCode, dirCode).Neighbours;

		for(unsigned int n = 0; n < neighboursDirs.size(); ++n)
		{
			IntVector2 jumpPoint;
			if(IsMoveStraight(neighboursDirs[n]))
			{
				PROFILE_REGISTER_CLOCK(jumpSClock, "JumpStraight");
				PROFILE_START(jumpSClock);
				jumpPoint = JumpStraight(current, neighboursDirs[n], goal);
				PROFILE_END(jumpSClock);
			}
			else
			{
				PROFILE_REGISTER_CLOCK(jumpDClock, "JumpDiagonal");
				PROFILE_START(jumpDClock);
				jumpPoint = JumpDiagonal(current, neighboursDirs[n], goal);
				PROFILE_END(jumpDClock);
			}
			if(jumpPoint.X > 0)
				successors.push_back(jumpPoint);
		}
		PROFILE_END(maskClock);
	}

	void UniformGridVariedSizeJumpFinder::ComputeUnitMasks()
	{
		auto maskCIt = _masksMap.find(CodeMask(
			Directions::Center, _unitSize));
		bool masksComputed = maskCIt != _masksMap.end();

		_masks[Directions::Center] = &_masksMap[CodeMask(Directions::Center, _unitSize)];
		_masks[Directions::Down] = &_masksMap[CodeMask(Directions::Down, _unitSize)];
		_masks[Directions::DownLeft] = &_masksMap[CodeMask(Directions::DownLeft, _unitSize)];
		_masks[Directions::DownRight] = &_masksMap[CodeMask(Directions::DownRight, _unitSize)];
		_masks[Directions::Left] = &_masksMap[CodeMask(Directions::Left, _unitSize)];
		_masks[Directions::Right] = &_masksMap[CodeMask(Directions::Right, _unitSize)];
		_masks[Directions::Up] = &_masksMap[CodeMask(Directions::Up, _unitSize)];
		_masks[Directions::UpLeft] = &_masksMap[CodeMask(Directions::UpLeft, _unitSize)];
		_masks[Directions::UpRight] = &_masksMap[CodeMask(Directions::UpRight, _unitSize)];

		if(masksComputed == false)
		{
			// Compute new masks for this unit and add them to map
			PROFILE_REGISTER_CLOCK(maskClock, "ComputeUnitMasks");
			PROFILE_START(maskClock);

			
			//if(_unitSquare)
			//{
			//	// For square units, assume all masks of same size
			//	ComputeUnitMasksForStraightMoves();
			//	ComputeUnitMasksForDiagonalMoves();
			//}
			//else
			//{
				ComputeUnitMasksForStraightMoves();
				ComputeUnitMasksForDiagonalMoves();
			//}

			// Compute codes map for all masks
			// For center/up/down, left/right ... mask are same, so copy maps
			ComputeCellCodesMapForMask(Directions::Center);
			_perMaskCodesMap[CodeMask(Directions::Down, _unitSize)] = 
				_perMaskCodesMap[CodeMask(Directions::Center, _unitSize)];
			_perMaskCodesMap[CodeMask(Directions::Up, _unitSize)] = 
				_perMaskCodesMap[CodeMask(Directions::Center, _unitSize)];

			ComputeCellCodesMapForMask(Directions::Left);
			_perMaskCodesMap[CodeMask(Directions::Right, _unitSize)] = 
				_perMaskCodesMap[CodeMask(Directions::Left, _unitSize)];
			
			ComputeCellCodesMapForMask(Directions::UpLeft);
			_perMaskCodesMap[CodeMask(Directions::DownRight, _unitSize)] = 
				_perMaskCodesMap[CodeMask(Directions::UpLeft, _unitSize)];
			
			ComputeCellCodesMapForMask(Directions::UpRight);
			_perMaskCodesMap[CodeMask(Directions::DownLeft, _unitSize)] = 
				_perMaskCodesMap[CodeMask(Directions::UpRight, _unitSize)];
			PROFILE_END(maskClock);
		}
	}

	void UniformGridVariedSizeJumpFinder::ComputeUnitMasksForStraightMoves()
	{
		// For even number center cell is on negative side, for odd its just center
		// So for odd minX = minY, for even minX = maxX - 1
		int minX = (_unitSize.X-1) / 2;
		int maxX = (_unitSize.X) / 2;
		int minY = (_unitSize.Y-1) / 2;
		int maxY = (_unitSize.Y) / 2;

		// For (0,0), (0,1), (0,-1) mask is rectangle = unit size
		// For (1,0), (-1,0) mask id rotated 90deg
		for(int y = -minY; y <= maxY; ++y)
		{
			for(int x = -minX; x <= maxX; ++x)
			{
				_masks[Directions::Center]->Cells.push_back(IntVector2(x,y));
				_masks[Directions::Up]->Cells.push_back(IntVector2(x,y));
				_masks[Directions::Down]->Cells.push_back(IntVector2(x,y));
				_masks[Directions::Left]->Cells.push_back(IntVector2(y,x));
				_masks[Directions::Right]->Cells.push_back(IntVector2(y,x));
			}
		}

		_masks[Directions::Center]->Center = IntVector2(minX, minY);
		_masks[Directions::Up]->Center = IntVector2(minX, minY);
		_masks[Directions::Down]->Center = IntVector2(minX, minY);
		_masks[Directions::Left]->Center = IntVector2(minY, minX);
		_masks[Directions::Right]->Center = IntVector2(minY, minX);

		_masks[Directions::Center]->Size = _unitSize;
		_masks[Directions::Up]->Size = _unitSize;
		_masks[Directions::Down]->Size = _unitSize;
		_masks[Directions::Left]->Size = IntVector2(_unitSize.Y, _unitSize.X);
		_masks[Directions::Right]->Size = IntVector2(_unitSize.Y, _unitSize.X);
	}

	void UniformGridVariedSizeJumpFinder::ComputeUnitMasksForDiagonalMoves()
	{
		// Find only one diagonal mask -> rotated 45deg counter-clockwise (up-left)
		// For diagonal move rect is rotated by 45deg
		// AABB of such roated is always square of size (w+h)/sqrt(2):
		float a = (_unitSize.X + _unitSize.Y) * 0.7f;
		// Round it for size in cells ( some cells partialy occupied by rect
		// will be omiited, but they are covered by small enouch part )
		int cellSize = a - (int)a < 0.5f ? (int)a : (int)a+1;
		// Bounds of AABB ( as (dx,dy) from center cell )
		int xmin, xmax, ymin, ymax;
		if( (cellSize & 1) == 0 )
		{
			// If AABB cellsize is even, center cell is on top-left of real center, so
			// bounds are from csize/2 - 1 to csize/2
			xmin = ymin = cellSize/2 - 1;
			xmax = ymax = cellSize/2;
		}
		else
		{
			// If AABB cellsize is odd, center cell is on real center, so
			// bounds are from csize/2 to csize/2 (inted)
			xmin = ymin = xmax = ymax = cellSize/2;
		}

		// To find which cells from AABB should be considered occupied
		// ( assume occupied if is covered by rect in at least half )
		// need to find cells with centers within rect, so for each cell
		// find center and test if its inside rect

		// For even csize, cell (dx,dy) have center in (dx-0.5, dy-0.5) from real center
		float corr = (cellSize & 1) == 0 ? -0.5f : 0.f;
		for(int dx = -xmin; dx <= xmax; ++dx)
		{
			for(int dy = -ymin; dy <= ymax; ++dy)
			{
				// Normal center c = (dx+corr, dy+corr)
				// Center in roated coords : d = ( 0.7(cx - cy), 0.7(cx+cy+2corr) )
				Vector2 d( 0.7f * ((float)dx - (float)dy), 0.7f * ((float)dx + (float)dy + 2*corr));
				// Check if is inside
				if( !( d.x < -((float)_unitSize.X)*0.5f || 
					d.x > ((float)_unitSize.X)*0.5f ||
					d.y < -((float)_unitSize.Y)*0.5f ||
					d.y > ((float)_unitSize.Y)*0.5f) )
				{
					_masks[Directions::UpLeft]->Cells.push_back(IntVector2(dx, dy));
				}
			}
		}
		_masks[Directions::UpLeft]->Center = IntVector2(xmin, ymin);
		_masks[Directions::UpLeft]->Size = IntVector2(cellSize, cellSize);

		// Fill other directions
		_masks[Directions::DownRight]->Size = _masks[Directions::UpRight]->Size = 
			_masks[Directions::DownLeft]->Size = _masks[Directions::UpLeft]->Size;
		_masks[Directions::DownRight]->Center = _masks[Directions::UpRight]->Center = 
			_masks[Directions::DownLeft]->Center = _masks[Directions::UpLeft]->Center;

		// For opposite direction mask is same
		for(unsigned int i = 0; i < _masks[Directions::UpLeft]->Cells.size(); ++i)
		{
			_masks[Directions::DownRight]->Cells.push_back(
				_masks[Directions::UpLeft]->Cells[i]);
		}

		// Mirror UL rot for DL and UR ( shift by 1 for even size, as x=0 is not in center )
		if( (cellSize & 1) == 0 )
		{
			for(unsigned int i = 0; i < _masks[Directions::UpLeft]->Cells.size(); ++i)
			{
				_masks[Directions::DownLeft]->Cells.push_back(IntVector2(
					-_masks[Directions::UpLeft]->Cells[i].X + 1, 
					_masks[Directions::UpLeft]->Cells[i].Y));

				_masks[Directions::UpRight]->Cells.push_back(
					_masks[Directions::DownLeft]->Cells[i]);
			}
		}
		else
		{
			for(unsigned int i = 0; i < _masks[Directions::UpLeft]->Cells.size(); ++i)
			{
				_masks[Directions::DownLeft]->Cells.push_back(IntVector2(
					-_masks[Directions::UpLeft]->Cells[i].X, 
					_masks[Directions::UpLeft]->Cells[i].Y));

				_masks[Directions::UpRight]->Cells.push_back(
					_masks[Directions::DownLeft]->Cells[i]);
			}
		}
	}

	void UniformGridVariedSizeJumpFinder::ComputeCellCodesMapForMask(int dirCode)
	{
		UnitMask* mask = _masks[dirCode];
		// Get min/max cell so that patches will be within map bounds
		unsigned int minCellX = mask->Center.X + 1;
		unsigned int minCellY = mask->Center.Y + 1;
		unsigned int maxCellX = _pathingMap->GetCellCountX() - mask->Size.X + mask->Center.X - 1;
		unsigned int maxCellY = _pathingMap->GetCellCountY() - mask->Size.Y + mask->Center.Y - 1;

		Array2d<int>* codeMap = new Array2d<int>(
			_pathingMap->GetCellCountY(), _pathingMap->GetCellCountX(), 0x1FF);

		for(unsigned int cy = minCellY; cy <= maxCellY; ++cy)
		{
			for(unsigned int cx = minCellX; cx <= maxCellX; ++cx)
			{
				const int code = ComputeMaskedPatchCode(cx, cy, *mask);
				codeMap->SetAt(cy, cx, code);
			}
		}

		_perMaskCodesMap[CodeMask(dirCode, _unitSize)] = codeMap;
	}

	void UniformGridVariedSizeJumpFinder::RecosntructPath(AstarNode* startNode, AstarNode* goalNode)
	{
		PROFILE_REGISTER_CLOCK(maskClock, "RecosntructPath");
		PROFILE_START(maskClock);
		_lastPath.clear();
		_lastPathReal.clear();

		// For even num of cells add whole cell size to center cell top-left to find real center
		// For odd add half the cell size
		float corrX, corrY;
		int dirCode;

		// Reserve minimal path lenght at start - assuming all moves are diagonal
		_lastPath.reserve((int)(_lastPathDistance * 0.7f));
		_lastPathReal.reserve(_lastPath.size());
		AstarNode* currentNode = goalNode;
		AstarNode* prevNode = currentNode->GetAncestor();
		while(prevNode != 0)
		{
			_lastPath.push_back(currentNode->GetCell());

			// Find direction of movement
			dirCode = FindMoveDirCode(currentNode->GetCell(), prevNode->GetCell());

			// Find center correction for unit direction
			corrX = 1.f - (float)(_masks[dirCode]->Size.X & 1) * 0.5f;
			corrY = 1.f - (float)(_masks[dirCode]->Size.Y & 1) * 0.5f;

			_lastPathReal.push_back(Vector2(
				((float)currentNode->GetCellX()+corrX) * _pathingMap->GetCellSize().x, 
				((float)currentNode->GetCellY()+corrY) * _pathingMap->GetCellSize().y));

			currentNode = prevNode;
			prevNode = currentNode->GetAncestor();
		}
		_lastPath.push_back(IntVector2(
			startNode->GetCellX(), startNode->GetCellY()));

		_lastPathReal.push_back(Vector2(
			((float)startNode->GetCellX()+0.5f) * _pathingMap->GetCellSize().x, 
			((float)startNode->GetCellY()+0.5f) * _pathingMap->GetCellSize().y));
		PROFILE_END(maskClock);
	}

	void UniformGridVariedSizeJumpFinder::UpdateAll()
	{
		for(int cy = 0; cy < _pathingMap->GetCellCountY(); ++cy)
		{
			for(int cx = 0; cx < _pathingMap->GetCellCountX(); ++cx)
			{
				UpdateCell(cx, cy);
			}
		}
	}

	void UniformGridVariedSizeJumpFinder::UpdateCell(int cellX, int cellY)
	{
		// Update all masks
		for(auto codeMapIt = _perMaskCodesMap.begin(); 
			codeMapIt != _perMaskCodesMap.end(); ++codeMapIt)
		{
			UnitMask& mask = _masksMap[codeMapIt->first];

			int minCellX = mask.Center.X + 1;
			int minCellY = mask.Center.Y + 1;
			int maxCellX = _pathingMap->GetCellCountX() - mask.Size.X + mask.Center.X - 1;
			int maxCellY = _pathingMap->GetCellCountY() - mask.Size.Y + mask.Center.Y - 1;

			if( cellX < minCellX || cellX > maxCellX ||
				cellY < minCellY || cellY > maxCellY )
			{
				// Unit cannot move there, so make an obstacle
				codeMapIt->second->SetAt(cellY, cellX, 0x1FF);
			}
			else
			{
				codeMapIt->second->SetAt(cellY, cellX, 
					ComputeMaskedPatchCode(cellX, cellY, mask));
			}

		}
	}
}