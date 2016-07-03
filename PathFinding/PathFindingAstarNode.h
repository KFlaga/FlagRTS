#pragma once

#include "PathFindingUtils.h"
#include <map>


namespace PathFinding
{
	class AstarNode
	{
	protected:
		union 
		{
			int _cellCode;
			struct
			{
				short _cellX;
				short _cellY;
			};
		};
		float _distFromStart; // Aka g
		float _totalCost; // Aka f = g + h
		AstarNode* _ancestor;
		short _queueIdx;
		bool _closed;

	public:
		AstarNode() :
			_cellX(-1), _cellY(-1), _closed(false), _ancestor(0) { }
		AstarNode(const int cellCode) :
			_cellCode(cellCode), _closed(false), _ancestor(0) { }
		AstarNode(const int cellCode, const float dist) :
			_cellCode(cellCode), _closed(false), _distFromStart(dist), _ancestor(0) { }
		AstarNode(const int cellCode, const float dist, AstarNode* anc) :
			_cellCode(cellCode), _closed(false), _distFromStart(dist), _ancestor(anc) { }
		AstarNode(const int cellCode, 
			const float dist, 
			AstarNode* ancestor, 
			const int targetX, 
			const int targetY,
			const float hMult) :
			_cellCode(cellCode), 
			_closed(false), 
			_distFromStart(dist), 
			_ancestor(ancestor)
		{
			ComputeTotalCost(targetX, targetY, hMult);
		}
		

		inline void SetCellCode(const int code)
		{
			_cellCode = code;
		}

		inline void SetCellPosition(const short x, const short y)
		{
			_cellX = x;
			_cellY = y;
		}

		inline int GetCellCode() const { return _cellCode; }
		inline short GetCellX() const { return _cellX; }
		inline short GetCellY() const { return _cellY; }
		inline IntVector2 GetCell() const { return IntVector2(_cellX, _cellY); }

		inline float GetHeuristicValue(const int targetX, const int targetY, const float heutisticMultiplier)
		{
			// 1) h equal to real distance from this cell to target cell
	//		return heutisticMultiplier*std::sqrtf(
	//			(targetX-_cellX)*(targetX-_cellX) + (targetY-_cellY)*(targetY-_cellY));
			// 2) h equal to min distance from this cell to target using only straight/diagonal moves
			float absX = (float)std::abs(targetX-_cellX);
			float absY = (float)std::abs(targetY-_cellY);
			return absX > absY ? 
				heutisticMultiplier * ( absY*0.41f + absX ) :
				heutisticMultiplier * ( absX*0.41f + absY );
		}

		inline void SetDistanceFromStart(const float dist)
		{
			_distFromStart = dist;
		}

		inline float GetDistanceFromStart() const
		{
			return _distFromStart;
		}

		inline void UpdateTotalCost(const float newDistanceFromTarget)
		{
			// Becouse heutisic remains the same just sub old dist and add new
			_totalCost -= _distFromStart;
			_totalCost += newDistanceFromTarget;
			_distFromStart = newDistanceFromTarget;
		}

		inline void ComputeTotalCost(const int targetX, const int targetY, const float heutisticMultiplier)
		{
			_totalCost = _distFromStart + GetHeuristicValue(targetX, targetY, heutisticMultiplier); 
		}

		inline float GetTotalCost() const
		{
			return _totalCost;
		}

		inline bool IsClosed() const { return _closed; }

		inline void Close() { _closed = true; }

		inline AstarNode* GetAncestor() const { return _ancestor; }
		inline void SetAncestor(AstarNode* anc) { _ancestor = anc; }

		inline static int ComputeCellCode(const int x, const int y) 
		{ 
			return x + (y<<16); 
		}

		struct NodeSorter
		{
		public:
			inline bool operator()(const AstarNode& node1, const AstarNode& node2)
			{
				return node1.GetTotalCost() < node2.GetTotalCost();
			}
		};

		struct NodePtrSorter
		{
		public:
			inline bool operator()(const AstarNode* node1, const AstarNode* node2)
			{
				return node1->GetTotalCost() < node2->GetTotalCost();
			}
		};

		void SetQueueArrayIndex(const short idx) { _queueIdx = idx; }
		short GetQueueArrayIndex() const { return _queueIdx; }
	};
}