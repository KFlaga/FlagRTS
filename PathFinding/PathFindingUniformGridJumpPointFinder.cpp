#include "PathFindingUniformGridJumpPointFinder.h"
#include "PathFindingUniformGridPathingMap.h"
#include "PathFindingAstarNodeQueue.h"
#include <map>
#include <Profiler.h>

namespace PathFinding
{
	UniformGridJumpPointFinder::UniformGridJumpPointFinder(UniformGridPathingMap* pathMap) :
		_pathingMap(pathMap),
		_maxCellX(pathMap->GetCellCountX()),
		_maxCellY(pathMap->GetCellCountY()),
		_codesMap(pathMap->GetCellCountX(), pathMap->GetCellCountY()),
		_resultMap(513, 11)
	{
		PrepareResultMap();
	}

#pragma region ALGORITHM
	IntVector2 UniformGridJumpPointFinder::JumpStraight(
		const IntVector2& from, const IntVector2& direction, const IntVector2& goal)
	{
		JumpResult jumpResult = ClearCell;
		IntVector2 jumpPoint(from);
		int cellCode; // Code depends on obstacles on patch
		int dirCode = CodeDirection(direction.X, direction.Y);

		while(true)
		{

			jumpPoint += direction;
			PROFILE_REGISTER_CLOCK(maskClock, "GetCellCode");
			PROFILE_START(maskClock);
			cellCode = _codesMap(jumpPoint.Y, jumpPoint.X);
			PROFILE_END(maskClock);
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

	IntVector2 UniformGridJumpPointFinder::JumpDiagonal(
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
			PROFILE_REGISTER_CLOCK(maskClock, "GetCellCode");
			PROFILE_START(maskClock);
			cellCode = _codesMap(jumpPoint.Y, jumpPoint.X);
			PROFILE_END(maskClock);
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

				PROFILE_REGISTER_CLOCK(maskClock, "GetCellCode");
				PROFILE_START(maskClock);
				cellCode = _codesMap(farJumpPoint.Y, farJumpPoint.X);
				PROFILE_END(maskClock);
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

				PROFILE_REGISTER_CLOCK(maskClock, "GetCellCode");
				PROFILE_START(maskClock);
				cellCode = _codesMap(farJumpPoint.Y, farJumpPoint.X);
				PROFILE_END(maskClock);
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

#undef min
#undef max

	inline void UniformGridJumpPointFinder::FindSuccessors(const IntVector2& current, 
		const IntVector2& parent, const IntVector2& goal, 
		PFArray<IntVector2>& successors)
	{

		PROFILE_REGISTER_CLOCK(maskClock, "GetCellCode");
		PROFILE_START(maskClock);
		int cellCode = _codesMap(current.Y, current.X);
		PROFILE_END(maskClock);
		// Find direction from parent - we know its straight or diagonal, so its simple
		int dirCode = CodeDirection(std::min(std::max(-1,current.X - parent.X), 1),
			std::min(std::max(-1,current.Y - parent.Y), 1));
		PFArray<IntVector2>& neighboursDirs = _resultMap(cellCode, dirCode).Neighbours;
		for(unsigned int n = 0; n < neighboursDirs.size(); ++n)
		{
			IntVector2 jumpPoint;
			if(IsMoveStraight(neighboursDirs[n]))
				jumpPoint = JumpStraight(current, neighboursDirs[n], goal);
			else
				jumpPoint = JumpDiagonal(current, neighboursDirs[n], goal);
			if(jumpPoint.X > 0)
				successors.push_back(jumpPoint);
		}
	}

	void UniformGridJumpPointFinder::FindPath(const IntVector2& beginCell, 
		const IntVector2& endCell)
	{
		_lastPathDistance = 0.f;
		AstarNodeQueue openSet;
		std::unordered_map<int, AstarNode> allNodes;

		int beginCode = AstarNode::ComputeCellCode(beginCell.X, beginCell.Y);
		int endCode = AstarNode::ComputeCellCode(endCell.X, endCell.Y);
		int nCode;
		AstarNode* nNode;
		std::unordered_map<int, AstarNode>::iterator nNodeIt;
		float neighbourDistance;

		// Insert first node to queue
		allNodes.insert(std::make_pair(beginCode, 
			AstarNode(beginCode, 0.f, (AstarNode*)0, endCell.X, endCell.Y, _heuristicMultiplier)));
		AstarNode* startNode = &allNodes[beginCode];
		startNode->SetAncestor(startNode);
		openSet.Insert(startNode);

		while(openSet.IsNotEmpty())
		{
			// While there's open node and end is not reached
			// Open most promising node
			AstarNode* currentNode = openSet.Top();
			if(currentNode->GetCellCode() == endCode)
			{
				// Next node is our goal so end
				_lastPathDistance = currentNode->GetDistanceFromStart();
				startNode->SetAncestor(0);
				RecosntructPath(startNode, currentNode);
				return;
			}

			// Close current node
			openSet.Pop();
			currentNode->Close();

			// Find jump points successing from this node
			PFArray<IntVector2> currentNodeSuccessors;
			FindSuccessors(IntVector2(currentNode->GetCellX(), currentNode->GetCellY()), 
				IntVector2(currentNode->GetAncestor()->GetCellX(), currentNode->GetAncestor()->GetCellY()), 
				endCell, currentNodeSuccessors);
			// For each jump point :
			for(unsigned int n = 0; n < currentNodeSuccessors.size(); ++n)
			{
				IntVector2& nPoint = currentNodeSuccessors[n];
				// Get code of jump point
				nCode = AstarNode::ComputeCellCode(nPoint.X, nPoint.Y);
				// Add to cNode DistFS dist from cNode to Point - it equals heuristic, as
				// moves are only straight/diagonal to jump points ( later -> add function ComputeDistanceS/D )
				neighbourDistance = currentNode->GetDistanceFromStart() + 
					currentNode->GetHeuristicValue(nPoint.X, nPoint.Y, 1.f);
				// Find if node is created -> if not then add it to open set
				nNodeIt = allNodes.find(nCode);
				if(nNodeIt != allNodes.end())
				{
					// Node is added, find its address
					nNode = &nNodeIt->second;
					if(nNode->IsClosed() == true)
						continue; // Node is already closed, so ignore it

					if(neighbourDistance >= nNode->GetDistanceFromStart()) 
						continue;// Better path already found

					// Path to neighboor node including currentNode is better, so update it
					nNode->SetAncestor(currentNode);
					nNode->UpdateTotalCost(neighbourDistance);
					openSet.UpdateCost(nNode);
				}
				else
				{
					// Create node and get its address
					nNode = &(allNodes.insert(std::make_pair(nCode, 
						AstarNode(nCode, neighbourDistance, currentNode,
						endCell.X, endCell.Y, _heuristicMultiplier))
						)).first->second;
					openSet.Insert(nNode);
				}
			}
		}
	}

	void UniformGridJumpPointFinder::RecosntructPath(AstarNode* startNode, AstarNode* goalNode)
	{
		_lastPath.clear();
		_lastPathReal.clear();
		// Reserve minimal path lenght at start - assuming all moves are diagonal
		_lastPath.reserve((int)(_lastPathDistance * 0.7f));
		_lastPathReal.reserve(_lastPath.size());
		AstarNode* currentNode = goalNode;
		while(currentNode->GetAncestor() != 0)
		{
			_lastPath.push_back(IntVector2(
				currentNode->GetCellX(), currentNode->GetCellY()));

			_lastPathReal.push_back(PFVector2(
				((float)currentNode->GetCellX()+0.5f) * _pathingMap->GetCellSize().x, 
				((float)currentNode->GetCellY()+0.5f) * _pathingMap->GetCellSize().y));

			currentNode = currentNode->GetAncestor();
		}
		_lastPath.push_back(IntVector2(
			startNode->GetCellX(), startNode->GetCellY()));
		_lastPathReal.push_back(PFVector2(
			((float)startNode->GetCellX()+0.5f) * _pathingMap->GetCellSize().x, 
			((float)startNode->GetCellY()+0.5f) * _pathingMap->GetCellSize().y));
	}

#pragma endregion

#pragma region PREPROCESSING
	void UniformGridJumpPointFinder::PrepareResultMap()
	{
		for(int nCode = 0; nCode <= 512; ++nCode)
		{
			int dirCode = CodeDirection(0,0);
			ComputeCellResultStartPoint(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(0,0));
			dirCode = CodeDirection(1,0);
			ComputeCellResultHorizontal(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(1,0));
			dirCode = CodeDirection(-1,0);
			ComputeCellResultHorizontal(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(-1,0));
			dirCode = CodeDirection(0,1);
			ComputeCellResultVertical(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(0,1));
			dirCode = CodeDirection(0,-1);
			ComputeCellResultVertical(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(0,-1));
			dirCode = CodeDirection(-1,-1);
			ComputeCellResultDiagTopLeft(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(-1,-1));
			dirCode = CodeDirection(1,1);
			ComputeCellResultDiagBotRight(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(1,1));
			dirCode = CodeDirection(-1,1);
			ComputeCellResultDiagBotLeft(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(-1,1));
			dirCode = CodeDirection(1,-1);
			ComputeCellResultDiagTopRight(_resultMap(nCode, dirCode), nCode, dirCode, IntVector2(1,-1));
		}
	}

	void UniformGridJumpPointFinder::PrepareCellCodesMap()
	{
		for(int y = 0; y < _maxCellY; ++y)
		{
			for(int x = 0; x < _maxCellX; ++x)
			{
				_codesMap.SetAt(y,x, ComputePatchCellCode(IntVector2(x,y)));
			}
		}
	}

	int UniformGridJumpPointFinder::ComputePatchCellCode(const IntVector2& center)
	{
		int cellCode = 0;
		int shift = 0;
		for(int dy = -1; dy <= 1; ++dy)
		{
			for(int dx = -1; dx <= 1; ++dx)
			{
				if( center.X + dx < 1 || 
					center.Y + dy < 1 ||
					center.X + dx >= _maxCellX - 1 ||
					center.Y + dy >= _maxCellY - 1 ||
					(_pathingMap->GetCell(center.Y + dy, center.X + dx)
					.Blocking & _filter.Groups) > 0 )
				{
					cellCode |= 1 << shift;
				}
				++shift;
			}
		}
		return cellCode;
	}

	void UniformGridJumpPointFinder::UpdateObstacle(
		const PFArray2d<CollisionFilter>& obstacle, const IntVector2& topLeftCell)
	{
		// Update all cells on obstacle + adjacent cells
		for(int cy = -1; cy <= obstacle.GetRowCount(); ++cy)
		{
			for(int cx = -1; cx <= obstacle.GetColumnCount(); ++cx)
			{
				UpdateCell(topLeftCell.X + cx, topLeftCell.Y + cy);
			}
		}
	}

	void UniformGridJumpPointFinder::UpdateCell(int cellX, int cellY)
	{
		if( cellX < 0 || cellX >= _pathingMap->GetCellCountX() ||
			cellY < 0 || cellY >= _pathingMap->GetCellCountY() )
		{
			// Unit cannot move there, so make an obstacle
			_codesMap.SetAt(cellY, cellX, 0x1FF);
		}
		else
		{
			_codesMap.SetAt(cellY, cellX, ComputePatchCellCode(IntVector2(cellX, cellY)));
		}
	}

	void UniformGridJumpPointFinder::ComputeCellResultHorizontal(CellResult& result,
		int nCode, int dirCode, const IntVector2& dir)
	{
		// First check for obstacles -> if there's one rest matters not
		// For d = (-1,0) its check for obstacles in the middle ( 0b000 111 000 )
		if( (dir.X == 1 && (nCode & 0x30) > 0) ||
			(dir.X == -1 && (nCode & 0x18) > 0))
		{
			result.Result = JumpResult::Obstacle;
			result.Neighbours.clear();
			return;
		}
		// Add neighbour in front
		result.Neighbours.push_back(IntVector2(dir.X,0));
		result.Result = JumpResult::ClearCell;

		if( dir.X == 1 )
		{
			// Check for [x10] or [1x0] ( on top )
			if(((nCode & 0x6) == 0x2) || ((nCode & 0x5) == 0x1) )
				result.Neighbours.push_back(IntVector2(dir.X,-1));
			// Check for [10x] ( on top )
			if((nCode & 0x3) == 0x1)
				result.Neighbours.push_back(IntVector2(0,-1));
			// Check for [x10] or [1x0] ( on bot )
			if(((nCode & 0x180) == 0x80) || ((nCode & 0x140) == 0x40) )
				result.Neighbours.push_back(IntVector2(dir.X,1));
			// Check for [10x] ( on bot )
			if((nCode & 0xC0) == 0x40)
				result.Neighbours.push_back(IntVector2(0,1));
		}
		else // dir.X == -1
		{
			// Check for [01x] or [0x1] ( on top )
			if(((nCode & 0x3) == 0x2) || ((nCode & 0x5) == 0x4) )
				result.Neighbours.push_back(IntVector2(dir.X,-1));
			// Check for [x01] ( on top )
			if((nCode & 0x6) == 0x4)
				result.Neighbours.push_back(IntVector2(0,-1));
			// Check for [01x] or [0x1] ( on bot )
			if(((nCode & 0xC0) == 0x80) || ((nCode & 0x140) == 0x100) )
				result.Neighbours.push_back(IntVector2(dir.X,1));
			// Check for [x01] ( on bot )
			if((nCode & 0x180) == 0x100)
				result.Neighbours.push_back(IntVector2(0,1));
		}

		if(result.Neighbours.size() > 1)
			result.Result = JumpResult::ForcedNeighbour;
	}

	void UniformGridJumpPointFinder::ComputeCellResultVertical(CellResult& result,
		int nCode, int dirCode, const IntVector2& dir)
	{
		// First check for obstacles -> if there's one rest matters not
		// For d = (-1,0) its check for obstacles in the middle ( 0b010 010 010 )

		if( (dir.Y == 1 && (nCode & 0x90) > 0) ||
			(dir.Y == -1 && (nCode & 0x12) > 0))
		{
			result.Result = JumpResult::Obstacle;
			result.Neighbours.clear(); // There's no neighbours, as there;s no chance it will be a jump point
			return;
		}
		// Add neighbour in front
		result.Neighbours.push_back(IntVector2(0,dir.Y));
		result.Result = JumpResult::ClearCell;

		if( dir.Y == 1 )
		{
			// Check for [...][1..][0..] or [1..][...][0..]
			if(((nCode & 0x48) == 0x8) || ((nCode & 0x41) == 0x1) )
				result.Neighbours.push_back(IntVector2(-1,dir.Y));
			// Check for [1..][0..][...]
			if((nCode & 0x9) == 0x1)
				result.Neighbours.push_back(IntVector2(-1,0));
			// Check for [...][..1][..0] or [..1][...][..0]
			if(((nCode & 0x120) == 0x20) || ((nCode & 0x104) == 0x4) )
				result.Neighbours.push_back(IntVector2(1,dir.Y));
			// Check for [..1][..0][...]
			if((nCode & 0x24) == 0x4)
				result.Neighbours.push_back(IntVector2(1,0));
		}
		else // dir.Y == -1
		{
			// Check for [0..][1..][...] or [0..][...][1..]
			if(((nCode & 0x9) == 0x8) || ((nCode & 0x41) == 0x40) )
				result.Neighbours.push_back(IntVector2(-1,dir.Y));
			// Check for [...][0..][1..]
			if((nCode & 0x48) == 0x40)
				result.Neighbours.push_back(IntVector2(-1,0));
			// Check for [..0][..1][...] or [..0][...][..1]
			if(((nCode & 0x24) == 0x20) || ((nCode & 0x104) == 0x100) )
				result.Neighbours.push_back(IntVector2(1,dir.Y));
			// Check for [...][..0][..1]
			if((nCode & 0x120) == 0x100)
				result.Neighbours.push_back(IntVector2(1,0));
		}

		if(result.Neighbours.size() > 1)
			result.Result = JumpResult::ForcedNeighbour;
	}

	void UniformGridJumpPointFinder::ComputeCellResultDiagTopRight(CellResult& result,
		int nCode, int dirCode, const IntVector2& dir)
	{
		// First check for obstacles -> if there's one rest matters not :
		// - on the diagonal 0b 000 010 100
		// - check if theres space for diagonal move, so : 0b 000 100 010 v 0b 010 001 000 
		if( ((nCode & 0x10) > 0) ||
			((nCode & 0x22) == 0x22) || ((nCode & 0x88) == 0x88))
		{
			result.Result = JumpResult::Obstacle;
			result.Neighbours.clear();
			return;
		}
		// Add neighbour in front
		result.Neighbours.push_back(IntVector2(dir.X, dir.Y));
		result.Result = JumpResult::ClearCell;
		// Now check if natural nb is free ( 0b 000 000 010 )
		if( (nCode & 0x2) == 0)
		{
			result.Neighbours.push_back(IntVector2(0,-1));
			// top is free, so check if left is filled - we'll have FN ( 0b 000 001 000 )
			// if also top-left is free ( 0b 000 000 001 )
			if( ((nCode & 0x9) == 0x8) )
			{
				result.Neighbours.push_back(IntVector2(-1,-1));
				result.Result = JumpResult::ForcedNeighbour;
			}
		}
		// Check if 2nd natural nb is free ( 0b 000 100 000 )
		if( (nCode & 0x20) == 0)
		{
			result.Neighbours.push_back(IntVector2(1,0));
			// top is free, so check if bot is filled - we'll have FN ( 0b 010 000 000 )
			// if also bot-right is free ( 0b 100 000 000 )
			if( ((nCode & 0x180) == 0x80) )
			{
				result.Neighbours.push_back(IntVector2(1,1));
				result.Result = JumpResult::ForcedNeighbour;
			}
		}
	}


	void UniformGridJumpPointFinder::ComputeCellResultDiagBotLeft(CellResult& result,
		int nCode, int dirCode, const IntVector2& dir)
	{
		// First check for obstacles -> if there's one rest matters not :
		// - on the diagonal 0b 001 010 000
		// - check if theres space for diagonal move, so : 0b 000 100 010 v 0b 010 001 000 
		if( ((nCode & 0x10) > 0) ||
			((nCode & 0x22) == 0x22) || ((nCode & 0x88) == 0x88))
		{
			result.Result = JumpResult::Obstacle;
			result.Neighbours.clear();
			return;
		}
		// Add neighbour in front
		result.Neighbours.push_back(IntVector2(dir.X, dir.Y));
		result.Result = JumpResult::ClearCell;
		// Now check if natural nb is free ( 0b 000 001 000 )
		if( (nCode & 0x8) == 0)
		{
			result.Neighbours.push_back(IntVector2(-1,0));
			// filled ( 0b 000 000 010 ) + free ( 0b 000 000 001 )
			if( ((nCode & 0x3) == 0x2) )
			{
				result.Neighbours.push_back(IntVector2(-1,-1));
				result.Result = JumpResult::ForcedNeighbour;
			}
		}
		// Check if 2nd natural nb is free ( 0b 010 000 000 )
		if( (nCode & 0x80) == 0)
		{
			result.Neighbours.push_back(IntVector2(0,1));
			// filled ( 0b 000 100 000 ) + free ( 0b 100 000 000 )
			if( ((nCode & 0x120) > 0x20) )
			{
				result.Neighbours.push_back(IntVector2(1,1));
				result.Result = JumpResult::ForcedNeighbour;
			}
		}
	}


	void UniformGridJumpPointFinder::ComputeCellResultDiagTopLeft(CellResult& result,
		int nCode, int dirCode, const IntVector2& dir)
	{
		// First check for obstacles -> if there's one rest matters not :
		// - on the diagonal 0b 000 010 001
		// - check if theres space for diagonal move, so : 0b 000 001 010 v 0b 010 100 000 
		if( ((nCode & 0x10) > 0) ||
			((nCode & 0xA) == 0xA) || ((nCode & 0xA0) == 0xA0))
		{
			result.Result = JumpResult::Obstacle;
			result.Neighbours.clear();
			return;
		}
		// Add neighbour in front
		result.Neighbours.push_back(IntVector2(dir.X, dir.Y));
		result.Result = JumpResult::ClearCell;
		// Now check if natural nb is free ( 0b 000 000 010 )
		if( (nCode & 0x2) == 0)
		{
			result.Neighbours.push_back(IntVector2(0,-1));
			// filled ( 0b 000 001 000 ) + free ( 0b 000 000 100 )
			if( ((nCode & 0x24) == 0x20) )
			{
				result.Neighbours.push_back(IntVector2(1,-1));
				result.Result = JumpResult::ForcedNeighbour;
			}
		}
		// Check if 2nd natural nb is free ( 0b 000 001 000 )
		if( (nCode & 0x8) == 0)
		{
			result.Neighbours.push_back(IntVector2(-1,0));
			// filled ( 0b 010 000 000 ) + free ( 0b 001 000 000 )
			if( ((nCode & 0xC0) == 0x80) )
			{
				result.Neighbours.push_back(IntVector2(-1,1));
				result.Result = JumpResult::ForcedNeighbour;
			}
		}
	}

	// CHECK
	void UniformGridJumpPointFinder::ComputeCellResultDiagBotRight(CellResult& result,
		int nCode, int dirCode, const IntVector2& dir)
	{
		// First check for obstacles -> if there's one rest matters not :
		// - on the diagonal 0b 100 010 000
		// - check if theres space for diagonal move, so : 0b 000 001 010 v 0b 010 100 000 
		if( ((nCode & 0x010) > 0) ||
			((nCode & 0xA) == 0xA) || ((nCode & 0xA0) == 0xA0))
		{
			result.Result = JumpResult::Obstacle;
			result.Neighbours.clear();
			return;
		}
		// Add neighbour in front
		result.Neighbours.push_back(IntVector2(dir.X, dir.Y));
		result.Result = JumpResult::ClearCell;

		// Now check if natural nb is free ( 0b 010 000 000 )
		if( (nCode & 0x80) == 0)
		{
			result.Neighbours.push_back(IntVector2(0,1));
			// filled ( 0b 000 001 000 ) + free ( 0b 001 000 000 )
			if( ((nCode & 0x48) == 0x8) )
			{
				result.Neighbours.push_back(IntVector2(-1,1));
				result.Result = JumpResult::ForcedNeighbour;
			}
		}
		// Check if 2nd natural nb is free ( 0b 000 100 000 )
		if( (nCode & 0x20) == 0)
		{
			result.Neighbours.push_back(IntVector2(1,0));
			// filled ( 0b 000 000 010 ) + free ( 0b 000 000 100 )
			if( ((nCode & 0x6) == 0x2))
			{
				result.Neighbours.push_back(IntVector2(1,-1));
				result.Result = JumpResult::ForcedNeighbour;
			}
		}
	}

	void UniformGridJumpPointFinder::ComputeCellResultStartPoint(CellResult& result,
		int nCode, int dirCode, const IntVector2& direction)
	{
		result.Result = JumpResult::ClearCell;
		// Check for possible movement in all directions
		if( (nCode & 0x20) == 0 )
			result.Neighbours.push_back(IntVector2(1,0));
		if( (nCode & 0x8) == 0 )
			result.Neighbours.push_back(IntVector2(-1,0));
		if( (nCode & 0x80) == 0 )
			result.Neighbours.push_back(IntVector2(0,1));
		if( (nCode & 0x2) == 0 )
			result.Neighbours.push_back(IntVector2(0,-1));

		if( ((nCode & 0x4) == 0) &&
			((nCode & 0x22) != 0x22))
			result.Neighbours.push_back(IntVector2(1,-1));

		if( ((nCode & 0x40) == 0) &&
			((nCode & 0x88) != 0x88))
			result.Neighbours.push_back(IntVector2(-1,1));

		if( ((nCode & 0x100) == 0) &&
			((nCode & 0xA0) != 0xA0))
			result.Neighbours.push_back(IntVector2(1,1));

		if( ((nCode & 0x1) == 0) &&
			((nCode & 0xA) != 0xA))
			result.Neighbours.push_back(IntVector2(-1,-1));
	}

#pragma endregion

#pragma region DEBUG
#ifdef _DEBUG
#include "PathFindingIDebugDrawer.h"

	void UniformGridJumpPointFinder::FindPathDebug(const IntVector2& beginCell, 
		const IntVector2& endCell)
	{
		_lastPathDistance = 0.f;
		_debugNodes.clear();
		_debugJumpFarPaths.clear();
		_debugJumpPaths.clear();
		AstarNodeQueue openSet;
		std::unordered_map<int, AstarNode> allNodes;

		int beginCode = AstarNode::ComputeCellCode(beginCell.X, beginCell.Y);
		int endCode = AstarNode::ComputeCellCode(endCell.X, endCell.Y);
		int nCode;
		AstarNode* nNode;
		std::unordered_map<int, AstarNode>::iterator nNodeIt;
		float neighbourDistance;

		allNodes.insert(std::make_pair(beginCode, 
			AstarNode(beginCode, 0.f, (AstarNode*)0, endCell.X, endCell.Y, _heuristicMultiplier)));
		AstarNode* startNode = &allNodes[beginCode];
		startNode->SetAncestor(startNode);
		openSet.Insert(startNode);

		_lastExpandedNodes = 0;
		_lastVisitedNodes = 0;
		_lastJumps = 0;
		_lastJumpCalls = 0;

		while(openSet.IsNotEmpty())
		{
			++_lastExpandedNodes;
			AstarNode* currentNode = openSet.Top();
			if(currentNode->GetCellCode() == endCode)
			{
				_lastPathDistance = currentNode->GetDistanceFromStart();
				startNode->SetAncestor(0);
				RecosntructPath(startNode, currentNode);
				break;
			}

			openSet.Pop();
			currentNode->Close();

			PFArray<IntVector2> currentNodeSuccessors;
			FindSuccessors(IntVector2(currentNode->GetCellX(), currentNode->GetCellY()), 
				IntVector2(currentNode->GetAncestor()->GetCellX(), currentNode->GetAncestor()->GetCellY()), 
				endCell, currentNodeSuccessors);

			for(unsigned int n = 0; n < currentNodeSuccessors.size(); ++n)
			{
				++_lastVisitedNodes;
				IntVector2& nPoint = currentNodeSuccessors[n];
				nCode = AstarNode::ComputeCellCode(nPoint.X, nPoint.Y);
				neighbourDistance = currentNode->GetDistanceFromStart() + 
					currentNode->GetHeuristicValue(nPoint.X, nPoint.Y, 1.f);
				nNodeIt = allNodes.find(nCode);
				if(nNodeIt != allNodes.end())
				{
					nNode = &nNodeIt->second;
					if(nNode->IsClosed() == true)
						continue;
					if(neighbourDistance >= nNode->GetDistanceFromStart()) 
						continue;

					nNode->SetAncestor(currentNode);
					nNode->UpdateTotalCost(neighbourDistance);
					openSet.UpdateCost(nNode);
				}
				else
				{
					nNode = &(allNodes.insert(std::make_pair(nCode, 
						AstarNode(nCode, neighbourDistance, currentNode,
						endCell.X, endCell.Y, _heuristicMultiplier))
						)).first->second;
					openSet.Insert(nNode);
				}
			}
		}

		for(auto it = allNodes.begin(); it != allNodes.end(); ++it)
		{
			_debugNodes.push_back(DebugNode(IntVector2(it->second.GetCellX(), it->second.GetCellY()),
				IntVector2(it->second.GetAncestor()->GetCellX(), it->second.GetAncestor()->GetCellY()),
				it->second.GetDistanceFromStart(), it->second.GetTotalCost()));
		}
	}

	IntVector2 UniformGridJumpPointFinder::JumpDebug(
		const IntVector2& from, const IntVector2& direction, const IntVector2& goal)
	{
		_lastJumpCalls++;
		JumpResult jumpResult = ClearCell;
		IntVector2 jumpPoint(from);
		IntVector2 farJumpPoint;
		IntVector2 xDir(direction.X,0);
		IntVector2 yDir(0,direction.Y);
		int cellCode;
		int dirCode = CodeDirection(direction.X, direction.Y);
		int xDirCode = CodeDirection(xDir.X, xDir.Y);
		int yDirCode = CodeDirection(yDir.X, yDir.Y);

		while(true)
		{
			_lastJumps++;
			_debugJumpPaths.push_back(std::make_pair(
				jumpPoint, jumpPoint + direction));
			jumpPoint += direction;
			cellCode = _codesMap(jumpPoint.Y, jumpPoint.X);
			jumpResult = _resultMap(cellCode, dirCode).Result;

			if(jumpResult == Obstacle)
				return IntVector2(-1,-1);

			if(jumpResult == ForcedNeighbour ||
				jumpPoint == goal )
				return jumpPoint;

			if(IsMoveDiagonal(direction))
			{
				farJumpPoint.X = jumpPoint.X;
				farJumpPoint.Y = jumpPoint.Y;
				while(true)
				{
					_debugJumpFarPaths.push_back(std::make_pair(
						farJumpPoint, farJumpPoint + xDir));
					farJumpPoint += xDir;
					cellCode = _codesMap(farJumpPoint.Y, farJumpPoint.X);
					jumpResult = _resultMap(cellCode, xDirCode).Result;

					if(jumpResult == Obstacle)
						break;

					if(jumpResult == ForcedNeighbour ||
						farJumpPoint == goal )
						return jumpPoint;
				}
				farJumpPoint.X = jumpPoint.X;
				farJumpPoint.Y = jumpPoint.Y;
				while(true)
				{
					_debugJumpFarPaths.push_back(std::make_pair(
						farJumpPoint, farJumpPoint + yDir));
					farJumpPoint += yDir;
					cellCode = _codesMap(farJumpPoint.Y, farJumpPoint.X);
					jumpResult = _resultMap(cellCode, yDirCode).Result;

					if(jumpResult == Obstacle)
						break;

					if(jumpResult == ForcedNeighbour ||
						farJumpPoint == goal )
						return jumpPoint;
				}
			}
		}
		return IntVector2(-1,-1);
	}

#undef min
#undef max

	inline void UniformGridJumpPointFinder::FindSuccessorsDebug(const IntVector2& current, 
		const IntVector2& parent, const IntVector2& goal, 
		PFArray<IntVector2>& successors)
	{
		int cellCode = _codesMap(current.Y, current.X);
		int dirCode = CodeDirection(std::min(std::max(-1,current.X - parent.X), 1),
			std::min(std::max(-1,current.Y - parent.Y), 1));
		PFArray<IntVector2>& neighboursDirs = _resultMap(cellCode, dirCode).Neighbours;
		for(unsigned int n = 0; n < neighboursDirs.size(); ++n)
		{
			IntVector2 jumpPoint;
			jumpPoint = JumpDebug(current, neighboursDirs[n], goal);
			if(jumpPoint.X > 0)
				successors.push_back(jumpPoint);
		}
	}

	void DrawArrow(IDebugDrawer* drawer, const IntVector2& beginPos, const IntVector2& endPos, int size);

	void UniformGridJumpPointFinder::DrawLastPath(IDebugDrawer* drawer)
	{

	}

	void UniformGridJumpPointFinder::DrawSearchGraph(IDebugDrawer* drawer, bool drawJumpPath, bool drawJumpFarPath)
	{

	}

#endif
#pragma endregion
}