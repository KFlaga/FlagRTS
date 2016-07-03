#pragma once

namespace FlagRTS
{
	// Simple class that represent arbitary memory
	// and is castable to every type
	// It contains header which tells how big is this chunk
	// for deallocation purposes - 
	// when casted to other type this header is
	// placed before actual data
	struct MemoryChunk
	{
		struct _MCHeader
		{
			unsigned int size; // Size of actual data
			unsigned int count; // Count of elements for arrays
			unsigned int info; // Additional info
		} Header;

		MemoryChunk()
		{
			Header.size = 0;
			Header.count = 1;
			Header.info = 0;
		}

		template<typename T>
		operator T*() const
		{
			return reinterpret_cast<T*>(reinterpret_cast<size_t>(this) + sizeof(_MCHeader));
		}

		template<typename T>
		static MemoryChunk* FromPtr(T* ptr)
		{
			return reinterpret_cast<MemoryChunk*>(reinterpret_cast<size_t>(ptr) - sizeof(_MCHeader));
		}
	};
}