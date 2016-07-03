#pragma once

#include "Allocator.h"
#include "ObjectPool.h"
#include <map>

namespace FlagRTS
{

	// ObjectPoolAllocator used ObjectPools to allocate memory
	// For each size of allocation, different pool is used
	class ObjectPoolAllocator : public Allocator
	{
	private:
		std::map<size_t, ObjectsPool*> _pools;
		size_t _maxSize;

		int _items;
		size_t _memory;

	public:
		ObjectPoolAllocator() :
			_maxSize(100000u), 
			_items(0),
			_memory(0)
		{ }

		~ObjectPoolAllocator()
		{
			for(auto it = _pools.begin(); it != _pools.end(); ++it)
			{
				delete it->second;
			}
			_pools.clear();
		}

		Allocator* GetAllocator()
		{
			return this;
		}

		void FreeAllocator(Allocator* allocator)
		{

		}

		int GetAllocatedItems() const
		{
			return _items;
		}

		size_t GetAllocatedMemory() const
		{
			return _memory;
		}

		size_t GetReservedMemory() const
		{
			size_t memory = 0;
			for(auto it = _pools.begin(); it != _pools.end(); ++it)
			{
				memory += it->second->GetReservedMemory();
			}
			return memory;
		}

	protected:
		MemoryChunk* AllocInternal(const size_t size, const unsigned int count) 
		{ 
			_items++;
			_memory += size;
			const size_t chunkSize = size*count;
			auto poolIt = _pools.find(chunkSize);
			if(poolIt != _pools.end())
			{
				return poolIt->second->GetNewItem();
			}
			else
			{
				ObjectsPool* pool = new ObjectsPool(chunkSize, std::min(100u, _maxSize/size + 1u));
				_pools[chunkSize] = pool;
				return pool->GetNewItem();
			}
		}

		void DeallocInternal(MemoryChunk* mem)
		{
			_items--;
			_memory -= mem->Header.size;

			_pools[mem->Header.size]->FreeItem(mem);
		}

		void ReserveInternal(const size_t size, const unsigned int count)
		{
			const size_t chunkSize = size*count;
			auto poolIt = _pools.find(chunkSize);
			if(poolIt != _pools.end())
			{
				poolIt->second->Reserve(chunkSize);
			}
			else
			{
				ObjectsPool* pool = new ObjectsPool(chunkSize, std::min(100u, 100000u/size + 1u));
				pool->Reserve(chunkSize);
			}
		}
	};
}