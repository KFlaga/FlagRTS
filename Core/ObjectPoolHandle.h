#pragma once

#include "TypeDefs.h"
#include "MemoryChunk.h"

namespace FlagRTS
{
#define INVALID_INDEX_16 0xfffe
#define INVALID_INDEX_32 (uint32)0xfffefffe
#define INVALID_ADDRESS (size_t)UINTPTR_MAX

	// Handle to pool item, which stores data address as well as
	// source pool/node/index info
	struct ObjectPoolHandle
	{
	private:
		union OPHandle
		{
			uint32 fullHandle;
			uint16 partial[2];
		} _handle;

	public:
		ObjectPoolHandle()
		{
			Release();
		}

		ObjectPoolHandle(uint16 node, uint16 item)
		{ 
			SetNodeIdx(node);
			SetItemIdx(item);
		}

		ObjectPoolHandle(uint32 handle) 
		{ 
			SetHandle(handle);
		}

		ObjectPoolHandle(MemoryChunk* mem) 
		{ 
			SetHandle(mem->Header.info);
		}

		uint16 GetNodeIdx() const { return _handle.partial[0]; }
		uint16 GetItemIdx() const { return _handle.partial[1]; }
		uint32 GetHandle() const { return _handle.fullHandle; }

		void SetNodeIdx(uint16 node)  {  _handle.partial[0] = node; }
		void SetItemIdx(uint16 item)  {  _handle.partial[1] = item; }
		void SetHandle(uint32 handle)  {   _handle.fullHandle = handle; }

		void Release()
		{
			SetHandle(INVALID_INDEX_32);
		}

		bool IsValid() const 
		{ 
			return  _handle.fullHandle != INVALID_INDEX_32; 
		}

		operator uint32() const
		{
			return _handle.fullHandle;
		}

		bool operator==(const ObjectPoolHandle& h)
		{
			return GetHandle() == h.GetHandle();
		}

		bool operator!=(const ObjectPoolHandle& h)
		{
			return GetHandle() != h.GetHandle();
		}

		bool operator<(const ObjectPoolHandle& h)
		{
			return GetHandle() < h.GetHandle();
		}

		bool operator>(const ObjectPoolHandle& h)
		{
			return GetHandle() > h.GetHandle();
		}

		bool operator>=(const ObjectPoolHandle& h)
		{
			return GetHandle() >= h.GetHandle();
		}

		bool operator<=(const ObjectPoolHandle& h)
		{
			return GetHandle() <= h.GetHandle();
		}
	};
}