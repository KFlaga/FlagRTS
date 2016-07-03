#pragma once

#include "MemoryChunk.h"
#include <vector>

namespace FlagRTS
{
	// Nodes allocates and holds MemoryChunks
	class MemoryNode
	{
	public:
		MemoryNode(size_t size);
		~MemoryNode();

		// Returns amount of memory still free in this node
		size_t GetFreeMemory() const;
		// Returns largest chunk of memory that can be allocated in this node
		size_t GetLargestChunk() const;
		// Returns total size of node
		size_t GetTotalMemory() const;

		// Returns node chunk belongs to from header
		unsigned short GetNode(MemoryChunk* mem) const;
		// Returns position in node chunk belongs to from header
		unsigned short GetPosition(MemoryChunk* mem) const;
	};

	class MemoryPool
	{
	protected:
		std::vector<MemoryNode*> _nodes;

		unsigned int _memoryUsed; // How much memory is used by allocated objects ( for debug )
		unsigned int _memoryReserved; // How much memory

	public:
		MemoryPool(size_t nodeSize, unsigned int nodeCount);
		~MemoryPool();

		// Returns allocated memory chunk of given size
		MemoryChunk* Alloc(size_t size);
		// Frees previously allocated memory
		void Free(MemoryChunk* mem);
		// Ensures that at least 
		void Reserve(size_t size);

		unsigned int GetMemoryUsed() const { return _memoryUsed; }
		unsigned int GetMemoryReserved() const { return _memoryReserved; }
	};
}