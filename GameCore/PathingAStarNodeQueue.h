#pragma once

#include "PathingAStarNode.h"
#include <Array.h>

namespace FlagRTS
{
	// Specialized priority queue for Astar finder Nodes which
	// saves direct pointers to queue array and enables decreasing of node
	// after node is placed
	class AstarNodeQueue
	{
	private:
		typedef unsigned int uint;
		Array<AstarNode*> _heap;

	public:
		AstarNodeQueue()
		{
			_heap.reserve(32);
		}

		inline int Parent(const int childIdx) { return (childIdx-1) >> 1; }
		inline int Left(const int parentIdx) { return (parentIdx << 1) + 1; }
		inline int Right(const int parentIdx) { return (parentIdx << 1) + 2; }
		inline bool IsLeft(const int nodeIdx) { return (nodeIdx & 1) > 0; }
		inline bool IsRight(const int nodeIdx) { return (nodeIdx & 1) == 0; }

		// Inserts new node into queue
		void Insert(AstarNode* node);

		// Returns root node
		inline AstarNode* Top() const { return _heap[0]; }

		// Removes root node
		void Pop();

		// Updates node position in queue ( assumes node cost is lowered )
		void UpdateCost(AstarNode* node);

		// Clears queue
		inline void Clear() { _heap.clear(); }
		
		inline bool IsEmpty() const { return _heap.size() == 0; }
		inline bool IsNotEmpty() const { return _heap.size() > 0; }

		inline void Swap(AstarNode* node, const int newIdx)
		{
			_heap[node->GetQueueArrayIndex()] = _heap[newIdx];
			_heap[node->GetQueueArrayIndex()]->SetQueueArrayIndex(node->GetQueueArrayIndex());
			_heap[newIdx] = node;
			node->SetQueueArrayIndex(newIdx);
		}
	};

	inline void AstarNodeQueue::Insert(AstarNode* node)
	{
		node->SetQueueArrayIndex(_heap.size());
		_heap.push_back(node);

		int parentIdx;
		while(parentIdx = Parent(node->GetQueueArrayIndex()),
			node->GetQueueArrayIndex() > 0 && 
			_heap[parentIdx]->GetTotalCost() > node->GetTotalCost())
		{
			// While parent is larger than new child, swap them
			Swap(node, parentIdx);
		}
	}

	inline void AstarNodeQueue::Pop()
	{
		// Swap last node and root node and remove last
		AstarNode* node = _heap[_heap.size()-1];
		_heap[0] = node;
		node->SetQueueArrayIndex(0);
		_heap.pop_back();
		// Swap this node with larger child till both children are smaller or reach last level
		unsigned int leftIdx, rightIdx;
		while(true)
		{
			leftIdx = Left(node->GetQueueArrayIndex()); 
			rightIdx = Right(node->GetQueueArrayIndex());

			if(leftIdx >= _heap.size())
				break; // node have no children -> end
			if(rightIdx >= _heap.size())
				// node have only left child
				if(_heap[leftIdx]->GetTotalCost() < node->GetTotalCost())
					Swap(node, leftIdx); // Left child is larger, swap them
				else
					break; // The only child is smaller -> end
			else
				// Find smaller child
				if(_heap[leftIdx]->GetTotalCost() < _heap[rightIdx]->GetTotalCost())
					// Smaller is left, so compare left and parent
					if(_heap[leftIdx]->GetTotalCost() < node->GetTotalCost())
						Swap(node, leftIdx); // Left child is larger, swap them
					else
						break; // The smaller child is larger -> end
				else
					// Smaller is right, so compare right and parent
					if(_heap[rightIdx]->GetTotalCost() < node->GetTotalCost())
						Swap(node, rightIdx); // Left child is larger, swap them
					else
						break; // The smaller child is larger -> end
		}
	}

	inline void AstarNodeQueue::UpdateCost(AstarNode* node)
	{
		// Node distance is lowered, so swap with parent to repair heap
		int parentIdx;
		while(parentIdx = Parent(node->GetQueueArrayIndex()),
			node->GetQueueArrayIndex() > 0 && 
			_heap[parentIdx]->GetTotalCost() > node->GetTotalCost())
		{
			// While parent is larger than new child, swap them
			Swap(node, parentIdx);
		}
	}
}