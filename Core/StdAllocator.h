#pragma once

#include "Allocator.h"
#include <cstdlib>
#include "Profiler.h"

namespace FlagRTS
{
	class StdAllocator : public Allocator
	{
	protected:
		int _allocatedItems;
		int64 _allocatedMemory;

	public:
		virtual Allocator* GetAllocator()
		{
			return this;
		}

		virtual void FreeAllocator(Allocator* allocator) { }

		int GetAllocatedItems() const { return _allocatedItems; }
		int64 GetAllocatedMemory() const { return _allocatedMemory; }

	protected:
		MemoryChunk* AllocInternal(const size_t size, const unsigned int count) 
		{ 
			PROFILE_REGISTER_CLOCK(memoryAllocsClock, "MemoryAllocs");
			PROFILE_START(memoryAllocsClock);
			MemoryChunk* mem = reinterpret_cast<MemoryChunk*>(std::malloc(count*size + sizeof(MemoryChunk::_MCHeader)));
			mem->Header.count = count;
			mem->Header.size = size;
			PROFILE_END(memoryAllocsClock);
			return mem;
		}

		void DeallocInternal(MemoryChunk* mem)
		{
			PROFILE_REGISTER_CLOCK(memoryDeAllocsClock, "MemoryDeAllocs");
			PROFILE_START(memoryDeAllocsClock);
			std::free((void*)mem);
			PROFILE_END(memoryDeAllocsClock);
		}

		void ReserveInternal(const size_t size, const unsigned int count)
		{

		}

		MemoryChunk* AllocInternal_d(const size_t size, const unsigned int count, const char* file, int line, const char* type)
		{
			MemoryChunk* mem = Allocator::AllocInternal_d(size, count, file, line, type);
			++_allocatedItems;
			_allocatedMemory += size*count;
			return mem;
		}

		void DeallocInternal_d(MemoryChunk* mem, const char* file, int line, const char* type)
		{
			Allocator::DeallocInternal_d(mem, file, line, type);
			--_allocatedItems;
			_allocatedMemory -= mem->Header.count * mem->Header.size;
		}
	};
}