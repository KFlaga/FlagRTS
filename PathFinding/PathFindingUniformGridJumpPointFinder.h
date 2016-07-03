#pragma once

#include "PathFindingIPathFinder.h"

namespace PathFinding
{
	class UniformGridPathingMap;
	struct CollisionFilter;
	class AstarNode;
	class IDebugDrawer;

	class UniformGridJumpPointFinder : public IGlobalPathFinder
	{
	public:
		enum JumpResult
		{
			ClearCell = 0,
			ForcedNeighbour = 1,
			Obstacle = 2,
		};

		struct CellResult
		{
			JumpResult Result;
			PFArray<IntVector2> Neighbours;
		};

	protected:
		UniformGridPathingMap* _pathingMap;
		PFArray2d<int> _codesMap; // Created from pathing map
		// Maps [x=dircode,y=cellcode] to CellResults
		PFArray2d<CellResult> _resultMap;

		float _lastPathDistance;
		float _heuristicMultiplier;
		int _maxCellX;
		int _maxCellY;


	public:
		UniformGridJumpPointFinder(UniformGridPathingMap* pathMap);

		void FindPath(const IntVector2& beginCell, const IntVector2& endCell);

		void SetHeuristicMultiplier(const float eps) { _heuristicMultiplier = eps; }

		// 1) Version with precomputed cellcodes -> one filter per results
		// 2) Version with online cellcodes -> filter can be changed
		void SetBlockFilter(const int filter) { _filter = filter; }

		float GetLastPathDistance() const { return _lastPathDistance; }

		PFArray<IntVector2>& GetLastPath() { return _lastPath; }

		void UpdateAll()
		{
			PrepareCellCodesMap();
		}

		void UpdateCell(int cellX, int cellY);

		void UpdateObstacle(const PFArray2d<CollisionFilter>& obstacle, const IntVector2& topLeftCell);

	protected:
		virtual void RecosntructPath(AstarNode* startNode, AstarNode* goalNode);

		virtual IntVector2 JumpStraight(const IntVector2& from, const IntVector2& direction, const IntVector2& goal);
		virtual IntVector2 JumpDiagonal(const IntVector2& from, const IntVector2& direction, const IntVector2& goal);
		virtual void FindSuccessors(const IntVector2& current, 
			const IntVector2& parent, const IntVector2& goal, 
			PFArray<IntVector2>& sucessors);

		void PrepareResultMap();
		void PrepareCellCodesMap();

		int ComputePatchCellCode(const IntVector2& center);

		void ComputeCellResultStartPoint(CellResult& result,
			int cellCode, int dirCode, const IntVector2& direction);
		void ComputeCellResultHorizontal(CellResult& result,
			int cellCode, int dirCode, const IntVector2& direction);
		void ComputeCellResultVertical(CellResult& result,
			int cellCode, int dirCode, const IntVector2& direction);

		void ComputeCellResultDiagTopLeft(CellResult& result,
			int cellCode, int dirCode, const IntVector2& direction);
		void ComputeCellResultDiagBotRight(CellResult& result,
			int cellCode, int dirCode, const IntVector2& direction);
		void ComputeCellResultDiagTopRight(CellResult& result,
			int cellCode, int dirCode, const IntVector2& direction);
		void ComputeCellResultDiagBotLeft(CellResult& result,
			int cellCode, int dirCode, const IntVector2& direction);

		int CodeDirection(int dx, int dy)
		{
			// Direction is ([-1,0,1],[-1,0,1])
			// Code is 4bits ([y][x]) -> [00,01,10] = [-1,0,1]
			// Max value is [1010] = 10
			return (dx+1) | ((dy+1)<<2); 
		}

		int CodePoint(int x, int y)
		{
			return x + (y<<16);
		}

		bool IsMoveStraight(const IntVector2& direction)
		{
			return direction.X * direction.Y == 0;
		}

		bool IsMoveDiagonal(const IntVector2& direction)
		{
			return direction.X * direction.Y != 0;
		}

		enum Directions : int
		{
			Center = 1 | (1<<2),
			Up = 1,
			Down = 1 | (2<<2),
			Left = (1<<2),
			Right = 2 | (1<<2),
			UpLeft = 0,
			DownLeft = (2<<2),
			UpRight = 2,
			DownRight = 2 | (2<<2),
		};

		IntVector2 FindMoveDirection(const IntVector2& currentCell, const IntVector2& previousCell)
		{
			// Possible moves are straight or diagonal only, so if (curr - prev) is bounded by
			// [-1,1], we'll get one of (-1,0,1) indicating direction
			return IntVector2(std::min(std::max(-1,currentCell.X - previousCell.X), 1),
				std::min(std::max(-1,currentCell.Y - previousCell.Y), 1));
		}

		int FindMoveDirCode(const IntVector2& currentCell, const IntVector2& previousCell)
		{
			// Possible moves are straight or diagonal only, so if (curr - prev) is bounded by
			// [-1,1], we'll get one of (-1,0,1) 
			return CodeDirection(std::min(std::max(-1,currentCell.X - previousCell.X), 1),
				std::min(std::max(-1,currentCell.Y - previousCell.Y), 1));
		}

#pragma region DEBUG
#ifdef _DEBUG
	protected: 
		int _lastExpandedNodes;
		int _lastVisitedNodes;
		int _lastJumps;
		int _lastJumpCalls;

		struct DebugNode
		{
			IntVector2 Cell; 
			IntVector2 Ancestor;
			float CostFromStart;
			float TotalCost;
			DebugNode(const IntVector2& cell, const IntVector2& anc, float costg, float costf) :
				Cell(cell), Ancestor(anc), CostFromStart(costg), TotalCost(costf) { }
		};
		PFArray<DebugNode> _debugNodes;
		PFArray<std::pair<IntVector2,IntVector2>> _debugJumpPaths;
		PFArray<std::pair<IntVector2,IntVector2>> _debugJumpFarPaths;

		void FindSuccessorsDebug(const IntVector2& current, const IntVector2& parent, const IntVector2& goal, PFArray<IntVector2>& sucessors);
		IntVector2 JumpDebug(const IntVector2& from, const IntVector2& direction, const IntVector2& goal);

	public:
		void FindPathDebug(const IntVector2& beginCell, const IntVector2& endCell);

		int GetLastExpandedNodeCount() const { return _lastExpandedNodes; }
		int GetLastJumpCount() const { return _lastJumps; }
		int GetLastJumpCallCount() const { return _lastJumpCalls; }

		// Draws last computed path -> jump points in path and directions to them
		void DrawLastPath(IDebugDrawer* drawer);
		// Draws full search graph:
		// - all expanded jump points with associated costs and directions from ancestors
		// - if enabled draws all jumps made in one direction to find jump nodes ( or reach obstacle )
		// - if enabled draws additional jumps made during diagonal moves to find possible jump points
		void DrawSearchGraph(IDebugDrawer* drawer, bool drawJumpPath = true, bool drawJumpFarDiagonalPath = false);

		PFArray<DebugNode>& GetLastExpandedNodes() { return _debugNodes; }
		PFArray<std::pair<IntVector2,IntVector2>>& GetJumpPaths() { return _debugJumpPaths; }
		PFArray<std::pair<IntVector2,IntVector2>>& GetJumpFarPaths() { return _debugJumpFarPaths; }

#endif
#pragma endregion
	};
}